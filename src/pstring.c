/*--------------------------------------------------------------------
 * Copyright © 2012-2016 James Hunt <jamesodhunt@gmail.com>.
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

#include "pstring.h"

#include <assert.h>

extern wchar_t wide_indent_char;

wchar_t *
char_to_wchar (const char *str)
{
	const char  *p;
	wchar_t     *wstr = NULL;
	size_t       len;
	size_t       bytes;

	assert (str);

	mbstate_t ps;

	memset(&ps, 0, sizeof (ps));

	len = mbsrtowcs (NULL, &str, 0, &ps);
	if (len <= 0)
		return NULL;

	/* include space for terminator */
	bytes = (1 + len) * sizeof (wchar_t);

	wstr = malloc (bytes);
	if (! wstr)
		return NULL;

	p = str;

	memset(&ps, 0, sizeof (ps));

	if (mbsrtowcs (wstr, &p, len, &ps) != len)
		goto error;

	/* ensure it's terminated */
	wstr[len] = L'\0';

	return wstr;

error:
	free (wstr);
	return NULL;
}

char *
wchar_to_char (const wchar_t *wstr)
{
	char    *str = NULL;
	size_t   len;
	size_t   bytes;
	size_t   ret;

	assert (wstr);

	len = wcslen (wstr);

	/* determine number of MBS (char) bytes requires to hold the
	 * wchar_t string.
	 */
	bytes = wcstombs (NULL, wstr, len);
	if (! bytes)
		return NULL;

	str = calloc (bytes + 1, sizeof (char));
	if (! str)
		return NULL;

	/* actually perform the conversion */
	ret = wcstombs (str, wstr, bytes);

	if (! ret)
		goto error;

	return str;

error:
	free (str);
	return NULL;
}

pstring *
pstring_new (void)
{
	pstring *pstr = NULL;

	pstr = calloc (1, sizeof (pstring));
	if (! pstr)
		return NULL;

	pstr->len = 0;
	pstr->size = 0;
	pstr->buf = NULL;

	return pstr;
}

pstring *
pstring_create (const wchar_t *str)
{
	pstring *pstr = NULL;

	assert (str);

	pstr = pstring_new ();

	if (! pstr)
		return NULL;

	pstr->buf = wcsdup (str);
	if (! pstr->buf) {
		pstring_free (pstr);
		return NULL;
	}

	/* include the L'\0' terminator */
	pstr->len = 1 + wcslen (pstr->buf);

	pstr->size = pstr->len * sizeof (wchar_t);

	return pstr;
}

void
pstring_free (pstring *str)
{
	assert (str);

	if (str->buf)
		free (str->buf);

	free (str);
}

pstring *
char_to_pstring (const char *str)
{
	pstring  *pstr = NULL;
	wchar_t  *s;

	assert (str);

	s = char_to_wchar (str);
	if (! s)
		return NULL;

	pstr = pstring_create (s);

	free (s);

	return pstr;
}

char *
pstring_to_char (const pstring *str)
{
	assert (str);

	return wchar_to_char (str->buf);
}

/**
 * pstring_chomp:
 *
 * Remove trailing extraneous newlines and indent_chars from @str.
 **/
void
pstring_chomp (pstring *str)
{
	size_t    len;
	int       removable = 0;
	wchar_t  *p;

	assert (str);

	/* Unable to add '\n' in this scenario */
	if (str->len < 2)
		return;

	for (p = str->buf+str->len-1;
            *p == L'\n' || *p == wide_indent_char;
			p--, removable++)
		;

	/* Chop string at the appropriate place after first adding a new
	 * newline.
	 */
	if (removable > 1) {
		len = str->len - (removable-1);
		str->buf[len-1] = L'\n';
		str->buf[len] = L'\0';
		str->len = len;
	}
}

/**
 * pstring_compress:
 *
 * Remove lines composed entirely of whitespace from @str.
 *
 * This is required specifically for '--output=text' which in some
 * scenarios generates lines comprising pure whitespace. This is
 * unnecessary and results from the fact that when an
 * ELEMENT_TYPE_OBJECT_* is encountered, formatting is applied for the
 * previously seen element, but sometimes such "objects" should be
 * invisible.
 **/
void
pstring_compress (pstring **wstr, wchar_t remove_char)
{
	wchar_t  *from;
	wchar_t  *to;
	wchar_t  *p;
	wchar_t  *start;
	size_t    count = 0;
	size_t    blanks = 0;
	size_t    new_len;
	size_t    bytes;

	assert (wstr);

	to = from = (*wstr)->buf;
	assert (from);

	while (to && *to) {
again:
		while (*to == L'\n' && *(to+1) == L'\n') {
			/* skip over blank lines */
			to++;
			blanks++;
		}

		start = to;

		while (*to == remove_char) {
			/* skip runs of contiguous characters */
			to++;
			count++;
		}

		if (to != start) {
			/* Only start consuming NLs at the end of a
			 * contiguous run *iff* there was more than a
			 * single removed char. This is a heuristic to
			 * avoid removing valid entries for example env
			 * vars that are set to nul are shown as:
			 *
			 *  'var: '
			 *
			 * Shudder.
			 */
			if (*to == L'\n' && to != start+1) {
				while (*to == L'\n') {
					/* consume the NL at the end of the contiguous run */
					to++;
				}

				/* check to ensure that we haven't entered a new line
				 * containing another block of chars to remove.
				 */
				if (*to == remove_char)
					goto again;

				blanks++;

			} else  {
				/* not a full line so backtrack */
				to = start;
				count = 0;
			}
		}

		*from++ = *to++;
	}

	/* terminate */
	*from = L'\0';

	if (blanks || count) {
		new_len = (*wstr)->len - (blanks + count);

		bytes = new_len * sizeof (wchar_t);

		p = realloc ((*wstr)->buf, bytes);
		assert (p);

		(*wstr)->buf = p;

		(*wstr)->buf[new_len-1] = L'\0';

		(*wstr)->len = new_len;
		(*wstr)->size = bytes;
	}
}

