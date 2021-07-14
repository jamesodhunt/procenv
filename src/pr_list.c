/*--------------------------------------------------------------------
 * Copyright (c) 2012-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 * Description: Generic list handling routines.
 *--------------------------------------------------------------------
 */

#include <pr_list.h>
#include <string.h>
#include <assert.h>

/**
 * pr_list_new:
 *
 * @data: data pointer to store in node.
 *
 * Create a new list entry.
 *
 * Returns: New PRList or NULL on error.
 **/
PRList *
pr_list_new (void *data)
{
	PRList *list;

	list = (PRList *)calloc (1, sizeof (PRList));

	if (! list)
		return NULL;

	list->next = list->prev = list;

	list->data = data;

	return list;
}

int
pr_list_empty(PRList *list)
{
    assert (list);

    return list->next == list && list->prev == list;
}

/**
 * pr_list_append:
 *
 * @list: list to operate on,
 * @entry: new PRList to add.
 *
 * Add @entry after @list entry.
 *
 * Returns: Newly-added entry.
 **/
PRList *
pr_list_append (PRList *list, PRList *entry)
{
	assert (list);
	assert (entry);

	entry->next = list->next;
	entry->prev = list;

	list->next->prev = entry;
	list->next = entry;

	return entry;
}

/**
 * pr_list_append_str:
 *
 * @list: list to operate on,
 * @str: string value to add to new entry.
 *
 * Create entry containing @str and add it after @list.
 *
 * Returns: New entry.
 **/
PRList *
pr_list_append_str (PRList *list, const char *str)
{
    size_t len;

    assert (list);
    assert (str);

    len = strlen (str);

    return pr_list_appendn_str (list, str, len);
}

/**
 * pr_list_appendn_str:
 *
 * @list: list to operate on,
 * @str: string value to add to new entry,
 * @len: length of @str to add to entry.
 *
 * Create entry containing @str and add it after @list.
 *
 * Returns: New entry.
 **/
PRList *
pr_list_appendn_str (PRList *list, const char *str, size_t len)
{
	char    *s;
	PRList  *entry;

	assert (list);
	assert (str);

	s = strndup (str, 1+len);
	assert (s);

	entry = pr_list_new (s);
	assert (entry);

	pr_list_append (list, entry);

	return entry;
}

/**
 * pr_list_prepend:
 *
 * @list: list to operate on,
 * @entry: new PRList to add.
 *
 * Add @entry before @list entry.
 *
 * Returns: Newly-added entry.
 **/
PRList *
pr_list_prepend (PRList *list, PRList *entry)
{
	assert (list);
	assert (entry);

	entry->next = list;
	entry->prev = list->prev;

	list->prev->next = entry;
	list->prev = entry;

	return entry;
}

/**
 * pr_list_prepend_str:
 *
 * @list: list to operate on,
 * @str: string value to add to new entry.
 *
 * Create entry containing @str and add it before @list.
 *
 * Returns: New entry.
 **/
PRList *
pr_list_prepend_str (PRList *list, const char *str)
{
	size_t len;

	assert (list);
	assert (str);

	len = strlen (str);

	return pr_list_prependn_str (list, str, len);
}

/**
 * pr_list_prependn_str:
 *
 * @list: list to operate on,
 * @str: string value to add to new entry,
 * @len: length of @str to add to entry.
 *
 * Create entry containing @str and add it before @list.
 *
 * Returns: New entry.
 **/
PRList *
pr_list_prependn_str (PRList *list, const char *str, size_t len)
{
	char    *s;
	PRList  *entry;

	assert (list);
	assert (str);

	s = strndup (str, 1+len);
	assert (s);

	entry = pr_list_new (s);
	assert (entry);

	pr_list_prepend (list, entry);

	return entry;
}

int
pr_list_cmp_str(PRList *la, PRList *lb)
{
    const char *a, *b;

    assert (la);
    assert (lb);

    a = (const char *)la->data;
    b = (const char *)lb->data;

    if (!a && !b) {
        return 0;
    } else if (!a && b) {
        return -1;
    } else if (a && !b) {
        return 1;
    } else {
        int ret = strcmp(a, b);
        return ret;
    }
}

#define pr_list_next(list, direction) \
    direction > 0 ? (list)->next : (list)->prev

static PRList *
pr_list_add_sorted_internal(PRList *list,
            PRList *entry,
            PRListCmp cmp,
            int prepend)
{
	PRList *p;
    PRList *start;
    int direction;

    assert (list);
    assert (entry);
    assert (cmp);

    PRList *(*handler)(PRList *list, PRList *entry);

    handler = prepend ? pr_list_prepend : pr_list_append;

    /* 1 being forwards, -1 backwards */
    direction = prepend ? 1 : -1;

    if (direction > 0) {
        start = list->next;
    } else {
        start = list->prev;
    }

    for (p = start; p != list; p = pr_list_next(p, direction)) {
        if (cmp (entry, p) < 0)
            return handler(p, entry);
    }

    return handler(list, entry);
}

PRList *
pr_list_prepend_sorted(PRList *list,
            PRList *entry,
            PRListCmp cmp)
{
    return pr_list_add_sorted_internal (list, entry, cmp, 1);
}

PRList *
pr_list_append_sorted(PRList *list,
            PRList *entry,
            PRListCmp cmp)
{
    return pr_list_add_sorted_internal (list, entry, cmp, 0);
}

PRList *
pr_list_prepend_str_sorted(PRList *list,
            PRList *entry)
{
    return pr_list_prepend_sorted(list, entry, pr_list_cmp_str);
}

PRList *
pr_list_append_str_sorted(PRList *list,
            PRList *entry)
{
    return pr_list_append_sorted(list, entry, pr_list_cmp_str);
}

/**
 * pr_list_remove:
 *
 * @list: list to operate on.
 *
 * Remove @entry from its containing list.
 *
 * Returns: Removed entry.
 **/
PRList *
pr_list_remove (PRList *entry)
{
	assert (entry);

	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;

	/* No longer attached to list */
	entry->next = entry->prev = entry;

	return entry;
}

/**
 * pr_list_foreach_visit:
 *
 * @list: list to operate on,
 * @visitor: function to run on every element on @list.
 *
 * Visit every entry in @list passing them to @visitor.
 **/
void
pr_list_foreach_visit (PRList *list, PRListVisitor visitor)
{
	PRList *p;

	assert (list);

	p = list;

	do {
		if (visitor)
			visitor (p);

		p = p->next;
	} while (p != list);
}

/**
 * pr_list_foreach_rev_visit:
 *
 * @list: list to operate on,
 * @visitor: function to run on every element on @list.
 *
 * Visit every entry in @list in reverse passing them to @visitor.
 **/
void
pr_list_foreach_rev_visit (PRList *list, PRListVisitor visitor)
{
	PRList *p;

	assert (list);

	p = list->prev;

	do {
		if (visitor)
			visitor (p);

		p = p->prev;
	} while (p != list->prev);
}

/**
 * pr_list_visitor_str:
 *
 * @entry: list entry to operate on.
 *
 * Display data in @entry as a string.
 **/
void
pr_list_visitor_str (PRList *entry)
{
    assert (entry);

    printf ("entry: addr=%p, prev=%p, next=%p, data=%p ('%s')\n",
		    (void *)entry,
		    (void *)entry->prev,
		    (void *)entry->next,
		    (void *)entry->data,
		    entry->data ? (char *)entry->data : "");
    fflush (NULL);
}
