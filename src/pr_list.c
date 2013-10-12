#include <pr_list.h>
#include <string.h>
#include <assert.h>

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

PRList *
pr_list_append (PRList *list, PRList *entry)
{
	assert (list);
	assert (entry);

	entry->next = list->next;
	entry->prev = list;

	list->next->prev = list->next = entry;

	return entry;
}

PRList *
pr_list_prepend (PRList *list, PRList *entry)
{
	assert (list);
	assert (entry);

	entry->next = list;
	entry->prev = list->prev;

	list->prev->next = list->prev = entry;

	return entry;
}

/* Add @str to new @entry in @list. Returns @entry */
PRList *
pr_list_prepend_str (PRList *list, const char *str)
{
	char    *s;
	PRList  *entry;

	assert (list);
	assert (str);

	s = strdup (str);
	assert (s);

	entry = pr_list_new (s);
	assert (entry);

	pr_list_prepend (list, entry);

	return entry;
}

PRList *
pr_list_remove (PRList *entry)
{
	assert (entry);

	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;

	/* No long attached to list */
	entry->next = entry->prev = entry;

	return entry;
}

#define PR_LIST_FOREACH_STR(list) \
	pr_list_foreach_visit (list, pr_list_visitor_str)

#define PR_LIST_FOREACH_REV_STR(list) \
	pr_list_foreach_rev_visit (list, pr_list_visitor_str)

/*
 * Visit each node in @list and pass it to @visitor.
 */
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
