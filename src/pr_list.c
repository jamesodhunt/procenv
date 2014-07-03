/*--------------------------------------------------------------------
 * procenv - generic list handling routines.
 *
 * Copyright 2012-2014 James Hunt.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
