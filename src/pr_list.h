#ifndef _PROCENV_LIST
#define _PROCENV_LIST

#include <stdio.h>
#include <stdlib.h>

typedef struct list {
	void         *data;
	struct list  *next;
	struct list  *prev;
} PRList;

typedef void (*PRListVisitor) (PRList *entry);

PRList *pr_list_new (void *data);

PRList *pr_list_append (PRList *list, PRList *entry);
PRList *pr_list_prepend (PRList *list, PRList *entry);
PRList *pr_list_prepend_str (PRList *list, const char *str);
PRList *pr_list_remove (PRList *entry);

void pr_list_foreach_visit (PRList *list, PRListVisitor visitor);
void pr_list_foreach_rev_visit (PRList *list, PRListVisitor visitor);

void pr_list_visitor_str (PRList *entry);

#define PR_LIST_FOREACH(list, iter) \
	for (PRList *iter = (list)->next; iter != (list); iter = iter->next)

#define PR_LIST_FOREACH_REV(list, iter) \
	for (PRList *iter = (list)->prev; iter != (list); iter = iter->prev)

#define PR_LIST_FOREACH_STR(list) \
	pr_list_foreach_visit (list, pr_list_visitor_str)

#define PR_LIST_FOREACH_REV_STR(list) \
	pr_list_foreach_rev_visit (list, pr_list_visitor_str)

#endif /* _PROCENV_LIST */
