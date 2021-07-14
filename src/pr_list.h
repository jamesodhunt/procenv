/*--------------------------------------------------------------------
 * Copyright (c) 2012-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 * Description: Generic list handling header.
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_LIST
#define _PROCENV_LIST

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

/* Double-linked list structure with no terminal node - a list of a
 * single item has next and prev pointers pointing "back round" to
 * itself.
 */
typedef struct list {
	void         *data;
	struct list  *next;
	struct list  *prev;
} PRList;

typedef void (*PRListVisitor) (PRList *entry);

typedef int (*PRListCmp) (PRList *a, PRList *b);

PRList *pr_list_new (void *data);

PRList *pr_list_append (PRList *list, PRList *entry);
PRList *pr_list_append_str (PRList *list, const char *str);
PRList *pr_list_appendn_str (PRList *list, const char *str, size_t len);

PRList *pr_list_prepend (PRList *list, PRList *entry);
PRList *pr_list_prepend_str (PRList *list, const char *str);
PRList *pr_list_prependn_str (PRList *list, const char *str, size_t len);

PRList *pr_list_prepend_str_sorted(PRList *list, PRList *entry);
PRList *pr_list_prepend_sorted(PRList *list, PRList *entry, PRListCmp cmp);

PRList *pr_list_append_str_sorted(PRList *list, PRList *entry);
PRList *pr_list_append_sorted(PRList *list, PRList *entry, PRListCmp cmp);

PRList *pr_list_remove (PRList *entry);

void pr_list_foreach_visit (PRList *list, PRListVisitor visitor);
void pr_list_foreach_rev_visit (PRList *list, PRListVisitor visitor);

void pr_list_visitor_str (PRList *entry);

#define PR_LIST_FOREACH(list, iter) \
	for (PRList *iter = (list)->next; iter != (list); iter = iter->next)

#define PR_LIST_FOREACH_SAFE(list, iter) \
	for (PRList *iter = (list)->next, *tmp = iter->next; \
		iter != (list); \
		iter = tmp, tmp = iter->next)

#define PR_LIST_FOREACH_REV(list, iter) \
	for (PRList *iter = (list)->prev; iter != (list); iter = iter->prev)

#define PR_LIST_FOREACH_STR(list) \
	pr_list_foreach_visit (list, pr_list_visitor_str)

#define PR_LIST_FOREACH_REV_STR(list) \
	pr_list_foreach_rev_visit (list, pr_list_visitor_str)

#endif /* _PROCENV_LIST */
