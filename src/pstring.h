/*--------------------------------------------------------------------
 * Copyright © 2016 James Hunt <jamesodhunt@gmail.com>.
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
