/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_PSTRING_H
#define _PROCENV_PSTRING_H

/* for wcsdup(3) */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

/**
 * @buf: string,
 * @len: number of _characters_ (*NOT* bytes!) in @buf,
 * @size: allocated size of @buf in bytes.
 **/
typedef struct procenv_string {
	wchar_t  *buf;
	size_t    len;
	size_t    size;
} pstring;

pstring *pstring_new (void);
pstring *pstring_create (const wchar_t *str);
pstring *char_to_pstring (const char *str);
char *pstring_to_char (const pstring *str);
void pstring_chomp (pstring *str);
void pstring_compress (pstring **wstr, wchar_t remove_char);
void pstring_free (pstring *str);
int encode_string (pstring **pstr);

wchar_t *char_to_wchar (const char *str);
char *wchar_to_char (const wchar_t *wstr);

#endif /* _PROCENV_PSTRING_H */
