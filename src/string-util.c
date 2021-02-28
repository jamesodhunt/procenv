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

#include "string-util.h"

/* append @src to @dest */
void
append (char **dest, const char *src)
{
	size_t  len;

	assert (dest);
	assert (src);

	len = strlen (src);

	appendn (dest, src, len);
}

/* Version of append() that operates on a wide string @dest and @src */
void
wappend (pstring **dest, const wchar_t *src)
{
	size_t  len;

	assert (dest);
	assert (src);

	len = wcslen (src);

	wappendn (dest, src, len);
}

/* Version of append() that operates on a wide string @dest
 * and multi-byte @src.
 */
void
wmappend (pstring **dest, const char *src)
{
	wchar_t  *wide_src = NULL;

	assert (dest);
	assert (src);

	wide_src = char_to_wchar (src);
	if (! wide_src)
		die ("failed to allocate space for wide string");

	wappend (dest, wide_src);

	free (wide_src);
}

/**
 * appendn:
 *
 * @dest: [output] string to append to,
 * @src: string to append to @dest,
 * @len: length of @new.
 *
 * Append first @len bytes of @new to @str,
 * ensuring result is nul-terminated.
 **/
void
appendn (char **dest, const char *src, size_t len)
{
	size_t  total;

	assert (dest);
	assert (src);

	if (! len)
		return;

	if (! *dest)
		*dest = strdup ("");
	if (! *dest)
		die ("failed to allocate space for string");

	/* +1 for terminating nul */
	total = strlen (*dest) + 1;

	total += len;

	*dest = realloc (*dest, total);
	assert (*dest);

	if (! *dest) {
		/* string is empty, so initialise the memory to avoid
		 * surprises with strncat() being unable to find the
		 * terminator!
		 */
		memset (*dest, '\0', total);
	}

	strncat (*dest, src, len);

	assert ((*dest)[total-1] == '\0');
}

/* Version of appendn() that operates on a wide string @dest and @src */
void
wappendn (pstring **dest, const wchar_t *src, size_t len)
{
	wchar_t  *p;
	size_t    total;
	size_t    bytes;

	assert (dest);
	assert (src);

	if (! len)
		return;

	if (! *dest)
		*dest = pstring_new ();
	if (! *dest)
		die ("failed to allocate space for pstring");

	total = (*dest)->len + len;

	/* +1 for terminating nul */
	bytes = (1 + total) * sizeof (wchar_t);

	p = realloc ((*dest)->buf, bytes);

	/* FIXME: turn into die() [all occurences!] */
	assert (p);

	(*dest)->buf = p;

	if (! (*dest)->len) {
		/* pstring is empty, so initialise the memory to avoid
		 * surprises with wcsncat() being unable to find the
		 * terminator!
		 */
		memset ((*dest)->buf, 0, bytes);
	}

	/* Used to check for overrun */
	(*dest)->buf[total] = L'\0';

	wcsncat ((*dest)->buf + (*dest)->len, src, len);

	/* update */
	(*dest)->len = total;
	(*dest)->size = bytes;

	/* check for overrun */
	assert ((*dest)->buf[total] == L'\0');
}

/* Version of appendn() that operates on a wide string @dest and
 * multi-byte @src.
 */
void
wmappendn (pstring **dest, const char *src, size_t len)
{
	wchar_t  *wide_src = NULL;

	assert (dest);
	assert (src);

	if (! len)
		return;

	wide_src = char_to_wchar (src);
	if (! wide_src)
		die ("failed to allocate space for wide string");

	wappendn (dest, wide_src, wcslen (wide_src));

	free (wide_src);
}

/* append @fmt and args to @dest */
void
appendf (char **dest, const char *fmt, ...)
{
	va_list   ap;

	assert (dest);
	assert (fmt);

	va_start (ap, fmt);

	appendva (dest, fmt, ap);

	va_end (ap);
}

/* Version of appendf() that operates on a wide string @dest
 * and @fmt.
 */
void
wappendf (pstring **dest, const wchar_t *fmt, ...)
{
	va_list  ap;

	assert (dest);
	assert (fmt);

	va_start (ap, fmt);

	wappendva (dest, fmt, ap);

	va_end (ap);
}

/* Version of appendf() that operates on a wide string @dest
 * and multi-byte @fmt.
 */
void
wmappendf (pstring **dest, const char *fmt, ...)
{
	wchar_t  *wide_fmt = NULL;
	va_list   ap;

	assert (dest);
	assert (fmt);

	wide_fmt = char_to_wchar (fmt);
	if (! wide_fmt)
		die ("failed to allocate memory for wide format");

	va_start (ap, fmt);

	wappendva (dest, wide_fmt, ap);

	va_end (ap);

	free (wide_fmt);
}

/* append @fmt and args to @dest */
void
appendva (char **dest, const char *fmt, va_list ap)
{
	int      ret;
	char    *new = NULL;
	char    *p;
	size_t   bytes;

	/* Start with a guess for how big we think the buffer needs to
	 * be.
	 */
	size_t   len = DEFAULT_ALLOC_GUESS_SIZE;

	assert (dest);
	assert (fmt);

	bytes = (1 + len) * sizeof (char);

	/* we could use vasprintf(3), but that's GNU-specific and hence
	 * not available everywhere we need it.
	 */
	new = malloc (bytes);
	if (! new)
		die ("failed to allocate space for string");

	memset (new, '\0', bytes);

	/* keep on increasing size of buffer until the translation
	 * succeeds.
	 */
	while (true) {
		va_list  ap_copy;

		va_copy (ap_copy, ap);
		ret = vsnprintf (new, len, fmt, ap_copy);
		va_end (ap_copy);

		if (ret < 0)
			die ("failed to format string");

		if ((size_t)ret < len) {
			/* now we have sufficient space */
			break;
		}

		/* Bump to allow one char to be written */
		len++;

		/* recalculate number of bytes */
		bytes = (1 + len) * sizeof (char);

		p = realloc (new, bytes);
		if (! p)
			die ("failed to allocate space for string");

		new = p;
	}

	if (*dest) {
		append (dest, new);
		free (new);
	} else {
		*dest = new;
	}
}

/* Version of appendva() that operates on a wide string @dest
 * and @fmt.
 */
void
wappendva (pstring **dest, const wchar_t *fmt, va_list ap)
{
	int       ret;
	wchar_t  *new = NULL;
	wchar_t  *p;
	size_t    bytes;
	va_list   ap_copy;

	/* Start with a guess for how big we think the buffer needs to
	 * be.
	 */
	size_t    len = DEFAULT_ALLOC_GUESS_SIZE;

	assert (dest);
	assert (fmt);

	bytes = (1 + len) * sizeof (wchar_t);

	new = malloc (bytes);
	if (! new)
		die ("failed to allocate space for wide string");

	memset (new, '\0', bytes);

	/* keep on increasing size of buffer until the translation
	 * succeeds.
	 */
	while (true) {
		va_copy (ap_copy, ap);
		ret = vswprintf (new, len, fmt, ap_copy);
		va_end (ap_copy);

		if ((size_t)ret < len) {
			/* now we have sufficient space, so update for
			 * actual number of bytes used (including the
			 * terminator!)
			 *
			 * Note that, conveniently, if the string is
			 * zero-characters long (ie ""), ret will be -1
			 * which we correct to 0.
			 */
			len = ret + 1;

			break;
		}

		/* Bump to allow one more wide-char to be written */
		len++;

		/* recalculate number of bytes */
		bytes = (1 + len) * sizeof (wchar_t);

		p = realloc (new, bytes);
		if (! p)
			die ("failed to allocate space for string");

		new = p;

		memset (new, '\0', bytes);
	}

	if (*dest) {
		wappend (dest, new);
		free (new);
	} else {
		wchar_t *n;

		/* recalculate number of bytes */
		bytes = (1 + len) * sizeof (wchar_t);

		/* compress */
		n = realloc (new, bytes);

		if (! n)
			die ("failed to reallocate space");

		new = n;

		(*dest) = pstring_new ();
		assert (*dest);
		(*dest)->buf = new;
		(*dest)->len = len;
		(*dest)->size = bytes;
	}
}

/* Version of appendva() that operates on a wide string @dest
 * and multi-byte @fmt.
 */
void
wmappendva (pstring **dest, const char *fmt, va_list ap)
{
	wchar_t  *wide_fmt = NULL;
	va_list   ap_copy;

	assert (dest);
	assert (fmt);

	wide_fmt = char_to_wchar (fmt);
	if (! wide_fmt)
		die ("failed to allocate memory for wide format");

	va_copy (ap_copy, ap);
	wappendva (dest, wide_fmt, ap_copy);
	va_end (ap_copy);

	free (wide_fmt);
}

/*
 * Append @src onto the end of @dest.
 */
void
pappend (pstring **dest, const pstring *src)
{
	size_t    total;
	size_t    bytes;
	wchar_t  *p;

	assert (dest);
	assert (src);

	if (! src->len)
		return;

	if (! *dest)
		*dest = pstring_new ();
	if (! *dest)
		die ("failed to allocate space for pstring");

	total = (*dest)->len + src->len;

	/* adjust since we only store _one_ of the string terminators
	 * from @dest and @src.
	 */
	total--;

	/* +1 for terminating nul */
	bytes = (1 + total) * sizeof (wchar_t);

	p = realloc ((*dest)->buf, bytes);

	/* FIXME: turn into die() [all occurences!] */
	assert (p);

	(*dest)->buf = p;

	wcsncat ((*dest)->buf + (*dest)->len, src->buf, src->len);

	/* update */
	(*dest)->len = total;
	(*dest)->size = bytes;

	/* Used to check for overrun */
	(*dest)->buf[total] = L'\0';
}

/**
 * @string: input,
 * @delimiter: field delimiter,
 * @compress: if true, ignore repeated contiguous delimiter characters,
 * @array: [output] array of fields, which this function will allocate.
 *
 * Notes: it is the callers responsibility to free @array
 * if the returned value is >0.
 *
 * Returns: number of fields in @string.
 **/
size_t
split_fields (const char *string, char delimiter, int compress, char ***array)
{
	const char  *p = NULL;
	const char  *start = NULL;
	size_t       count = 0;
	char        *elem;
	char       **new;

	assert (string);
	assert (delimiter);
	assert (array);

	*array = NULL;

	new = realloc ((*array), sizeof (char *) * (1+count));
	assert (new);

	new[0] = NULL;
	*array = new;

	p = string;

	while (p && *p) {
		/* skip leading prefix */
		while (compress && p && *p == delimiter)
			p++;

		if (! *p)
			break;

		/* found a field */
		count++;

		if (! compress)
			p++;

		/* skip over the field */
		start = p;
		while (p && *p && *p != delimiter)
			p++;

		elem = strndup (start, p-start);
		assert (elem);

		new = realloc ((*array), sizeof (char *) * (1+count));
		assert (new);

		new[count-1] = elem;
		*array = new;
	}

	return count;
}

