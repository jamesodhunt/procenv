/*--------------------------------------------------------------------
 * procenv - check(*) tests for generic list handing routines.
 *
 * (*) - check unit test program (see http://check.sourceforge.net/).
 *
 * Copyright © 2015 James Hunt <jamesodhunt@gmail.com>.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PROCENV_PR_LIST_TESTS
#include <pr_list.h>
#include <check.h>

/* tolerate old check versions */
#ifndef ck_assert_ptr_eq
#define ck_assert_ptr_eq(a, b) \
    assert ((a) == (b))
#endif

#ifndef ck_assert_ptr_ne
#define ck_assert_ptr_ne(a, b) \
    assert ((a) != (b))
#endif

#ifndef ck_assert_str_eq
#define ck_assert_str_eq(a, b) \
    assert (!strcmp (a, b))
#endif

#ifndef ck_assert_uint_eq
#define ck_assert_uint_eq(a, b) \
    assert ((uint)a == (uint)b)
#endif

/********************************************************************/

char buffer[8];
static int count = 0;

static void
pr_list_visit_save_data(PRList *entry)
{
    assert (entry);
    if (! entry->data)
        return;

    buffer[count] = *(const char *)entry->data;
    count++;
}

/********************************************************************/

START_TEST(test_pr_list_new)
{
    char *s = "foo bar";
    PRList *list;

    list = pr_list_new (s);
    ck_assert_ptr_ne (list, NULL);
    ck_assert_ptr_eq (list->next, list);
    ck_assert_ptr_eq (list->prev, list);

    free (list);
}
END_TEST

/********************************************************************/

START_TEST(test_pr_list_append)
{
    PRList *list, *p;
    PRList *entry1, *entry2, *entry3, *entry4, *entry5;

    /**********************************/
    /* create a list */
    list = pr_list_new (NULL);
    ck_assert_ptr_ne (list, NULL);
    ck_assert_ptr_eq (list->data, NULL);

    /**********************************/
    /* create some list entries */
    entry1 = pr_list_new ((void *)10);
    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);

    entry2 = pr_list_new ((void *)20);
    ck_assert_ptr_ne (entry2, NULL);
    ck_assert_uint_eq ((size_t)entry2->data, 20);

    entry3 = pr_list_new ((void *)30);
    ck_assert_ptr_ne (entry3, NULL);
    ck_assert_uint_eq ((size_t)entry3->data, 30);

    entry4 = pr_list_new ((void *)40);
    ck_assert_ptr_ne (entry4, NULL);
    ck_assert_uint_eq ((size_t)entry4->data, 40);

    entry5 = pr_list_new ((void *)50);
    ck_assert_ptr_ne (entry5, NULL);
    ck_assert_uint_eq ((size_t)entry5->data, 50);

    /**********************************/
    /* add 1st entry */
    p = pr_list_append (list, entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry1);

    /**********************************/
    /* remove 1st entry */
    p = pr_list_remove(entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, list);
    ck_assert_ptr_eq (list->prev, list);

    /**********************************/
    /* re-add 1st entry */
    p = pr_list_append (list, entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry1);

    /**********************************/
    /* add 2nd entry */
    p = pr_list_append (list, entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry1);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, entry2);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry1);

    /**********************************/
    /* add 3rd entry */
    p = pr_list_append (list, entry3);
    ck_assert_ptr_eq (p, entry3);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, list);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry1);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, entry2);

    ck_assert_ptr_eq (list->next, entry3);
    ck_assert_ptr_eq (list->prev, entry1);

    /**********************************/
    /* add 4th entry */
    p = pr_list_append (list, entry4);
    ck_assert_ptr_eq (p, entry4);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry3);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, entry4);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry1);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, entry2);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry1);

    /**********************************/
    /* add 5th entry */
    p = pr_list_append (list, entry5);
    ck_assert_ptr_eq (p, entry5);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry4);
    ck_assert_ptr_eq (entry5->prev, list);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry3);
    ck_assert_ptr_eq (entry4->prev, entry5);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, entry4);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry1);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, entry2);

    ck_assert_ptr_eq (list->next, entry5);
    ck_assert_ptr_eq (list->prev, entry1);

    /**********************************/
    /* remove 1st entry */
    p = pr_list_remove(entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry4);
    ck_assert_ptr_eq (entry5->prev, list);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry3);
    ck_assert_ptr_eq (entry4->prev, entry5);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, entry4);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry5);
    ck_assert_ptr_eq (list->prev, entry2);

    /**********************************/
    /* remove last entry */
    p = pr_list_remove(entry5);
    ck_assert_ptr_eq (p, entry5);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry3);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, entry4);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry2);

    /**********************************/
    /* remove middle entry */
    p = pr_list_remove(entry3);
    ck_assert_ptr_eq (p, entry3);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry2);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, entry4);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry2);

    /**********************************/
    /* remove entry 2 */
    p = pr_list_remove(entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry2);
    ck_assert_ptr_eq (entry2->prev, entry2);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry4);

    /**********************************/
    /* re-add entry 2 */
    p = pr_list_append (list, entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, entry2);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry4);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry4);

    /**********************************/
    /* re-remove entry 2 (it's now in a different position) */
    p = pr_list_remove(entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry2);
    ck_assert_ptr_eq (entry2->prev, entry2);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry4);

    /**********************************/
    /* remove entry 4 */
    p = pr_list_remove(entry4);
    ck_assert_ptr_eq (p, entry4);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry4);
    ck_assert_ptr_eq (entry4->prev, entry4);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry2);
    ck_assert_ptr_eq (entry2->prev, entry2);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, list);
    ck_assert_ptr_eq (list->prev, list);

    free (entry1);
    free (entry2);
    free (entry3);
    free (entry4);
    free (entry5);
    free (list);
}
END_TEST

/********************************************************************/

START_TEST(test_pr_list_prepend)
{
    PRList *list, *p;
    PRList *entry1, *entry2, *entry3, *entry4, *entry5;

    /**********************************/
    /* create a list */
    list = pr_list_new (NULL);
    ck_assert_ptr_ne (list, NULL);
    ck_assert_ptr_eq (list->data, NULL);

    /**********************************/
    /* create some list entries */
    entry1 = pr_list_new ((void *)10);
    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);

    entry2 = pr_list_new ((void *)20);
    ck_assert_ptr_ne (entry2, NULL);
    ck_assert_uint_eq ((size_t)entry2->data, 20);

    entry3 = pr_list_new ((void *)30);
    ck_assert_ptr_ne (entry3, NULL);
    ck_assert_uint_eq ((size_t)entry3->data, 30);

    entry4 = pr_list_new ((void *)40);
    ck_assert_ptr_ne (entry4, NULL);
    ck_assert_uint_eq ((size_t)entry4->data, 40);

    entry5 = pr_list_new ((void *)50);
    ck_assert_ptr_ne (entry5, NULL);
    ck_assert_uint_eq ((size_t)entry5->data, 50);

    /**********************************/
    /* add 1st entry */
    p = pr_list_prepend (list, entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry1);

    /**********************************/
    /* remove 1st entry */
    p = pr_list_remove(entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, list);
    ck_assert_ptr_eq (list->prev, list);

    /**********************************/
    /* re-add 1st entry */
    p = pr_list_prepend (list, entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry1);

    /**********************************/
    /* add 2nd entry */
    p = pr_list_prepend (list, entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry2);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry2);

    /**********************************/
    /* add 3rd entry */
    p = pr_list_prepend (list, entry3);
    ck_assert_ptr_eq (p, entry3);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, list);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry2);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry3);

    /**********************************/
    /* add 4th entry */
    p = pr_list_prepend (list, entry4);
    ck_assert_ptr_eq (p, entry4);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, entry3);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry4);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry2);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry4);

    /**********************************/
    /* add 5th entry */
    p = pr_list_prepend (list, entry5);
    ck_assert_ptr_eq (p, entry5);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, list);
    ck_assert_ptr_eq (entry5->prev, entry4);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry5);
    ck_assert_ptr_eq (entry4->prev, entry3);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry4);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, entry1);

    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry2);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry5);

    /**********************************/
    /* remove 1st entry */
    p = pr_list_remove(entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, list);
    ck_assert_ptr_eq (entry5->prev, entry4);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry5);
    ck_assert_ptr_eq (entry4->prev, entry3);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry4);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry5);

    /**********************************/
    /* remove last entry */
    p = pr_list_remove(entry5);
    ck_assert_ptr_eq (p, entry5);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, entry3);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry4);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry4);

    /**********************************/
    /* remove middle entry */
    p = pr_list_remove(entry3);
    ck_assert_ptr_eq (p, entry3);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, entry2);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry4);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry4);

    /**********************************/
    /* remove entry 2 */
    p = pr_list_remove(entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry2);
    ck_assert_ptr_eq (entry2->prev, entry2);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry4);

    /**********************************/
    /* re-add entry 2 */
    p = pr_list_prepend (list, entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry2);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, entry4);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry2);

    /**********************************/
    /* re-remove entry 2 (it's now in a different position) */
    p = pr_list_remove(entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry2);
    ck_assert_ptr_eq (entry2->prev, entry2);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry4);

    /**********************************/
    /* remove entry 4 */
    p = pr_list_remove(entry4);
    ck_assert_ptr_eq (p, entry4);

    ck_assert_uint_eq ((size_t)entry5->data, 50);
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    ck_assert_uint_eq ((size_t)entry4->data, 40);
    ck_assert_ptr_eq (entry4->next, entry4);
    ck_assert_ptr_eq (entry4->prev, entry4);

    ck_assert_uint_eq ((size_t)entry3->data, 30);
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    ck_assert_uint_eq ((size_t)entry2->data, 20);
    ck_assert_ptr_eq (entry2->next, entry2);
    ck_assert_ptr_eq (entry2->prev, entry2);

    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_uint_eq ((size_t)entry1->data, 10);
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    ck_assert_ptr_eq (list->next, list);
    ck_assert_ptr_eq (list->prev, list);

    free (entry1);
    free (entry2);
    free (entry3);
    free (entry4);
    free (entry5);
    free (list);
}
END_TEST

/********************************************************************/

START_TEST(test_pr_list_append_str)
{
    char *s[] = { "a", "b", "c"};
    PRList *list;
    PRList *entry1, *entry2, *entry3;

    list = pr_list_new (s[0]);
    ck_assert_ptr_ne (list, NULL);

    /* 1st entry */
    entry1 = pr_list_append_str (list, (void *)s[0]);
    ck_assert_ptr_ne (entry1, NULL);

    ck_assert_str_eq ((const char *)entry1->data, s[0]);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry1);

    /* 2nd entry */
    entry2 = pr_list_append_str (list, (void *)s[1]);
    ck_assert_ptr_ne (entry2, NULL);

    ck_assert_str_eq ((const char *)entry2->data, s[1]);
    ck_assert_ptr_eq (entry2->next, entry1);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, entry2);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry1);

    /* 3rd entry */
    entry3 = pr_list_append_str (list, (void *)s[2]);
    ck_assert_ptr_ne (entry3, NULL);

    ck_assert_str_eq ((const char *)entry3->data, s[2]);
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, list);

    ck_assert_ptr_eq (entry2->next, entry1);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, entry2);

    ck_assert_ptr_eq (list->next, entry3);
    ck_assert_ptr_eq (list->prev, entry1);

    free((char *)entry1->data);
    free(entry1);

    free((char *)entry2->data);
    free(entry2);

    free((char *)entry3->data);
    free(entry3);

    free (list);
}
END_TEST

/********************************************************************/

START_TEST(test_pr_list_prepend_str)
{
    char *s[] = { "a", "b", "c"};
    PRList *list;
    PRList *entry1, *entry2, *entry3;

    list = pr_list_new (s[0]);
    ck_assert_ptr_ne (list, NULL);

    /* 1st entry */
    entry1 = pr_list_prepend_str (list, (void *)s[0]);
    ck_assert_ptr_ne (entry1, NULL);

    ck_assert_str_eq ((const char *)entry1->data, s[0]);
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry1);

    /* 2nd entry */
    entry2 = pr_list_prepend_str (list, (void *)s[1]);
    ck_assert_ptr_ne (entry2, NULL);

    ck_assert_str_eq ((const char *)entry2->data, s[1]);
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, entry1);

    ck_assert_ptr_eq (entry1->next, entry2);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry2);

    /* 3rd entry */
    entry3 = pr_list_prepend_str (list, (void *)s[2]);
    ck_assert_ptr_ne (entry3, NULL);

    ck_assert_str_eq ((const char *)entry3->data, s[2]);
    ck_assert_ptr_eq (entry3->next, list);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, entry1);

    ck_assert_ptr_eq (entry1->next, entry2);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry3);

    free((char *)entry1->data);
    free(entry1);

    free((char *)entry2->data);
    free(entry2);

    free((char *)entry3->data);
    free(entry3);

    free (list);
}
END_TEST

/********************************************************************/

START_TEST(test_pr_list_prepend_str_sorted)
{
    PRList *list, *p;
    PRList *entry1, *entry2, *entry3, *entry4, *entry5;

    /**********************************/
    /* create a list */
    list = pr_list_new (NULL);
    ck_assert_ptr_ne (list, NULL);
    ck_assert_ptr_eq (list->data, NULL);

    /**********************************/
    /* create some list entries */
    entry1 = pr_list_new (strdup("a"));
    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_str_eq ((char *)entry1->data, "a");
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    entry2 = pr_list_new (strdup("b"));
    ck_assert_ptr_ne (entry2, NULL);
    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, entry2);
    ck_assert_ptr_eq (entry2->prev, entry2);

    entry3 = pr_list_new (strdup("c"));
    ck_assert_ptr_ne (entry3, NULL);
    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    entry4 = pr_list_new (strdup("d"));
    ck_assert_ptr_ne (entry4, NULL);
    ck_assert_str_eq ((char *)entry4->data, "d");
    ck_assert_ptr_eq (entry4->next, entry4);
    ck_assert_ptr_eq (entry4->prev, entry4);

    entry5 = pr_list_new (strdup("e"));
    ck_assert_ptr_ne (entry5, NULL);
    ck_assert_str_eq ((char *)entry5->data, "e");
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    /**********************************/
    /* add an entry */
    p = pr_list_prepend_str_sorted(list, entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry2);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "b");

    /**********************************/
    /* add another entry */
    p = pr_list_prepend_str_sorted(list, entry3);
    ck_assert_ptr_eq (p, entry3);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, list);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry3);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "bc");

    /**********************************/
    /* add yet another entry */
    p = pr_list_prepend_str_sorted(list, entry4);
    ck_assert_ptr_eq (p, entry4);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, entry4);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_str_eq ((char *)entry4->data, "d");
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, entry3);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry4);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "bcd");

    /**********************************/
    /* add the first entry */

    p = pr_list_prepend_str_sorted(list, entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_str_eq ((char *)entry1->data, "a");
    ck_assert_ptr_eq (entry1->next, entry2);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, entry1);

    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, entry4);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_str_eq ((char *)entry4->data, "d");
    ck_assert_ptr_eq (entry4->next, list);
    ck_assert_ptr_eq (entry4->prev, entry3);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry4);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "abcd");

    /**********************************/
    /* add the last entry */

    p = pr_list_prepend_str_sorted(list, entry5);
    ck_assert_ptr_eq (p, entry5);

    ck_assert_str_eq ((char *)entry1->data, "a");
    ck_assert_ptr_eq (entry1->next, entry2);
    ck_assert_ptr_eq (entry1->prev, list);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, entry3);
    ck_assert_ptr_eq (entry2->prev, entry1);

    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, entry4);
    ck_assert_ptr_eq (entry3->prev, entry2);

    ck_assert_str_eq ((char *)entry4->data, "d");
    ck_assert_ptr_eq (entry4->next, entry5);
    ck_assert_ptr_eq (entry4->prev, entry3);

    ck_assert_str_eq ((char *)entry5->data, "e");
    ck_assert_ptr_eq (entry5->next, list);
    ck_assert_ptr_eq (entry5->prev, entry4);

    ck_assert_ptr_eq (list->next, entry1);
    ck_assert_ptr_eq (list->prev, entry5);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "abcde");

    /**********************************/

    free ((char *)entry1->data);
    free (entry1);

    free ((char *)entry2->data);
    free (entry2);

    free ((char *)entry3->data);
    free (entry3);

    free ((char *)entry4->data);
    free (entry4);

    free ((char *)entry5->data);
    free (entry5);

    free (list);
}
END_TEST

/********************************************************************/

START_TEST(test_pr_list_append_str_sorted)
{
    PRList *list, *p;
    PRList *entry1, *entry2, *entry3, *entry4, *entry5;

    /**********************************/
    /* create a list */
    list = pr_list_new (NULL);
    ck_assert_ptr_ne (list, NULL);
    ck_assert_ptr_eq (list->data, NULL);

    /**********************************/
    /* create some list entries */
    entry1 = pr_list_new (strdup("a"));
    ck_assert_ptr_ne (entry1, NULL);
    ck_assert_str_eq ((char *)entry1->data, "a");
    ck_assert_ptr_eq (entry1->next, entry1);
    ck_assert_ptr_eq (entry1->prev, entry1);

    entry2 = pr_list_new (strdup("b"));
    ck_assert_ptr_ne (entry2, NULL);
    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, entry2);
    ck_assert_ptr_eq (entry2->prev, entry2);

    entry3 = pr_list_new (strdup("c"));
    ck_assert_ptr_ne (entry3, NULL);
    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, entry3);
    ck_assert_ptr_eq (entry3->prev, entry3);

    entry4 = pr_list_new (strdup("d"));
    ck_assert_ptr_ne (entry4, NULL);
    ck_assert_str_eq ((char *)entry4->data, "d");
    ck_assert_ptr_eq (entry4->next, entry4);
    ck_assert_ptr_eq (entry4->prev, entry4);

    entry5 = pr_list_new (strdup("e"));
    ck_assert_ptr_ne (entry5, NULL);
    ck_assert_str_eq ((char *)entry5->data, "e");
    ck_assert_ptr_eq (entry5->next, entry5);
    ck_assert_ptr_eq (entry5->prev, entry5);

    /**********************************/
    /* add an entry */
    p = pr_list_append_str_sorted(list, entry2);
    ck_assert_ptr_eq (p, entry2);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, list);

    ck_assert_ptr_eq (list->next, entry2);
    ck_assert_ptr_eq (list->prev, entry2);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "b");

    /**********************************/
    /* add another entry */
    p = pr_list_append_str_sorted(list, entry3);
    ck_assert_ptr_eq (p, entry3);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, list);

    ck_assert_ptr_eq (list->next, entry3);
    ck_assert_ptr_eq (list->prev, entry2);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "cb");

    /**********************************/
    /* add yet another entry */
    p = pr_list_append_str_sorted(list, entry4);
    ck_assert_ptr_eq (p, entry4);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, list);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, entry4);

    ck_assert_str_eq ((char *)entry4->data, "d");
    ck_assert_ptr_eq (entry4->next, entry3);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry2);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "dcb");

    /**********************************/
    /* add the first entry */

    p = pr_list_append_str_sorted(list, entry1);
    ck_assert_ptr_eq (p, entry1);

    ck_assert_str_eq ((char *)entry1->data, "a");
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, entry2);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, entry1);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, entry4);

    ck_assert_str_eq ((char *)entry4->data, "d");
    ck_assert_ptr_eq (entry4->next, entry3);
    ck_assert_ptr_eq (entry4->prev, list);

    ck_assert_ptr_eq (list->next, entry4);
    ck_assert_ptr_eq (list->prev, entry1);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "dcba");

    /**********************************/
    /* add the last entry */

    p = pr_list_append_str_sorted(list, entry5);
    ck_assert_ptr_eq (p, entry5);

    ck_assert_str_eq ((char *)entry1->data, "a");
    ck_assert_ptr_eq (entry1->next, list);
    ck_assert_ptr_eq (entry1->prev, entry2);

    ck_assert_str_eq ((char *)entry2->data, "b");
    ck_assert_ptr_eq (entry2->next, entry1);
    ck_assert_ptr_eq (entry2->prev, entry3);

    ck_assert_str_eq ((char *)entry3->data, "c");
    ck_assert_ptr_eq (entry3->next, entry2);
    ck_assert_ptr_eq (entry3->prev, entry4);

    ck_assert_str_eq ((char *)entry4->data, "d");
    ck_assert_ptr_eq (entry4->next, entry3);
    ck_assert_ptr_eq (entry4->prev, entry5);

    ck_assert_str_eq ((char *)entry5->data, "e");
    ck_assert_ptr_eq (entry5->next, entry4);
    ck_assert_ptr_eq (entry5->prev, list);

    ck_assert_ptr_eq (list->next, entry5);
    ck_assert_ptr_eq (list->prev, entry1);

    /* reset */
    memset (buffer, '\0', sizeof(buffer));
    count = 0;

    pr_list_foreach_visit (list, pr_list_visit_save_data);
    ck_assert_str_eq (buffer, "edcba");

    /**********************************/

    free ((char *)entry1->data);
    free (entry1);

    free ((char *)entry2->data);
    free (entry2);

    free ((char *)entry3->data);
    free (entry3);

    free ((char *)entry4->data);
    free (entry4);

    free ((char *)entry5->data);
    free (entry5);

    free (list);
}
END_TEST

/********************************************************************/

Suite *
pr_list_suite (void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create ("procenv pr_list");
    assert (s);

    tc_core = tcase_create ("core");

    /*******************************/
    /* Add each test */

    tcase_add_test (tc_core, test_pr_list_new);

    tcase_add_test (tc_core, test_pr_list_append);
    tcase_add_test (tc_core, test_pr_list_append_str);

    tcase_add_test (tc_core, test_pr_list_prepend);
    tcase_add_test (tc_core, test_pr_list_prepend_str);

    tcase_add_test (tc_core, test_pr_list_prepend_str_sorted);
    tcase_add_test (tc_core, test_pr_list_append_str_sorted);

    /*******************************/

    suite_add_tcase(s, tc_core);

    return s;
}

/********************************************************************/

int
main (int argc, char *argv[])
{
    Suite    *s;
    SRunner  *sr;
    int       number_failed;

    s = pr_list_suite ();
    sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (! number_failed) ? EXIT_SUCCESS : EXIT_FAILURE;
}
