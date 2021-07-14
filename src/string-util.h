/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_STRING_UTIL_H
#define _PROCENV_STRING_UTIL_H

/* for strndup(3) */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "pstring.h"
#include "output.h"
#include "util.h"

#define DEFAULT_ALLOC_GUESS_SIZE 8

/* operate on multi-bytes */
void append (char **dest, const char *src);
void appendn (char **dest, const char *src, size_t len);
void appendf (char **dest, const char *fmt, ...);
void appendva (char **dest, const char *fmt, va_list ap);

/* operate on pure wide-characters */
void wappend (pstring **dest, const wchar_t *src);
void wappendn (pstring **dest, const wchar_t *src, size_t len);
void wappendf (pstring **dest, const wchar_t *fmt, ...);
void wappendva (pstring **dest, const wchar_t *fmt, va_list ap);

/* operate on wide-characters, but using multi-byte formats */
void wmappend (pstring **dest, const char *src);
void wmappendn (pstring **dest, const char *src, size_t len);
void wmappendf (pstring **dest, const char *fmt, ...);
void wmappendva (pstring **dest, const char *fmt, va_list ap);

void pappend (pstring **dest, const pstring *src);
size_t split_fields (const char *string, char delimiter,
		     int compress, char ***array);

#endif /* _PROCENV_STRING_UTIL_H */
