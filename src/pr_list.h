/*--------------------------------------------------------------------
 * procenv - generic list handling header.
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
