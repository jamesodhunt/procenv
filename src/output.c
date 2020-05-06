/*--------------------------------------------------------------------
 * Copyright © 2016 James Hunt <jamesodhunt@ubuntu.com>.
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

#include <stdbool.h>
#include <syslog.h>
#include <assert.h>

#include "pr_list.h"
#include "string-util.h"
#include "output.h"

/* Default character to use for indent */
#define DEFAULT_INDENT_CHAR " "

/* If an indent is required, use this many INDENT_CHARs by default */
#define DEFAULT_INDENT_AMOUNT 2

#define PROCENV_DEFAULT_CRUMB_SEPARATOR ":"
#define PROCENV_DEFAULT_TEXT_SEPARATOR ": "

/*
 * XXX: WARNING! Gross magic number hack!
 *
 * - This value must equal the maximum number of entries in any particular
 *   OutputFormats translation table.
 *
 * - This must be updated to reflect the
 *   maximum values in any TranslateMapEntry, plus 1.
 */
#define TRANSLATE_MAP_ENTRIES    (5+1)

extern pstring *doc;
extern struct procenv_user user;

/**
 * output:
 *
 * Where to send output.
 **/
Output output = OUTPUT_STDOUT;

/**
 * output_format:
 *
 * Format output will be displayed in.
 **/
OutputFormat output_format = OUTPUT_FORMAT_TEXT;

/**
 * output_file:
 *
 * Name or output file to send output to if not NULL.
 **/
const char *output_file = NULL;

/**
 * output_file_append:
 *
 * If true, append to @output_file rather than truncating.
 **/
static int output_file_append = false;

/**
 * text_separator:
 *
 * Separator used for text output format to separate a name from a
 * value.
 **/
static const char *text_separator = PROCENV_DEFAULT_TEXT_SEPARATOR;

/**
 * output_fd:
 *
 * File descriptor associated with output_file.
 **/
int output_fd = -1;

static struct procenv_map output_map[] = {
	{ OUTPUT_FILE   , "file"     },
	{ OUTPUT_STDERR , "stderr"   },
	{ OUTPUT_STDOUT , "stdout"   },
	{ OUTPUT_SYSLOG , "syslog"   },
	{ OUTPUT_TERM   , "terminal" },

	{ 0             , NULL       }
};

static struct procenv_map output_format_map[] = {
	{ OUTPUT_FORMAT_TEXT  , "text"  },
	{ OUTPUT_FORMAT_CRUMB , "crumb" },
	{ OUTPUT_FORMAT_JSON  , "json"  },
	{ OUTPUT_FORMAT_XML   , "xml"   },

	{ 0                   , NULL    }
};

typedef struct translate_map_entry {
	wchar_t   from;
	wchar_t  *to;
} TranslateMapEntry;

typedef struct translate_table {
	OutputFormat output_format;
	TranslateMapEntry map[TRANSLATE_MAP_ENTRIES];
} TranslateTable;

/*
 * Note the gross hack to avoid need for flexible arrays.
 */
static TranslateTable translate_table[] = {
	{
		OUTPUT_FORMAT_XML,
		{
			{ L'\'', L"&apos;" },
			{ L'"', L"&quot;"  },
			{ L'&', L"&amp;"   },
			{ L'<', L"&lt;"    },
			{ L'>', L"&gt;"    },

			/* terminator */
			{ L'\0', NULL      }
		}
	},
	{
		OUTPUT_FORMAT_JSON,
		{
			{ L'"', L"\\\""    },
			{ L'\\', L"\\\\"   },

			/* XXX: the hack! */
			{ L'\0', NULL      },
			{ L'\0', NULL      },
			{ L'\0', NULL      },

			/* terminator */
			{ L'\0', NULL      }
		}
	},
};

/**
 * last_element: Type of previous element handled.
 **/
ElementType last_element = ELEMENT_TYPE_NONE;

/**
 * current_element: Type of element currently being handled.
 **/
ElementType current_element = ELEMENT_TYPE_NONE;

/**
 * crumb_list:
 *
 * List used to store breadcrumbs when OUTPUT_FORMAT_CRUMB being used.
 **/
static PRList *crumb_list = NULL;

/**
 * indent:
 *
 * Current output indent value.
 **/
static int indent = 0;

/**
 * indent_amount:
 *
 * Number of INDENT_CHARs to emit for an indent.
 **/
static int indent_amount = DEFAULT_INDENT_AMOUNT;

/**
 * indent_char, wide_indent_char:
 *
 * Character to use for indenting and wide-char equivalent.
 **/
const char *indent_char = DEFAULT_INDENT_CHAR;
wchar_t  wide_indent_char;

/**
 * crumb_separator:
 *
 * Separator used for text output format to separate a name from a
 * value.
 **/
const char *crumb_separator = PROCENV_DEFAULT_CRUMB_SEPARATOR;

void
header (const char *name)
{
	assert (name);
	common_assert ();

	section_open (name);
}

void
footer (void)
{
	common_assert ();
	section_close ();
}

/**
 * master_footer:
 *
 * @doc: document to write footer to.
 *
 * Main footer which is displayed once.
 **/
void
master_footer (pstring **doc)
{
	common_assert ();

	switch (output_format) {

	case OUTPUT_FORMAT_CRUMB: /* FALL */
	case OUTPUT_FORMAT_TEXT:
		/* Tweak */
		wappend (doc, L"\n");
		break;

	case OUTPUT_FORMAT_JSON:
		object_close (false);

		/* Tweak */
		wappend (doc, L"\n");
		break;

	case OUTPUT_FORMAT_XML:
		/* Tweak */
		wappend (doc, L"\n");
		dec_indent ();
		wappendf (doc, L"</%s>\n", PACKAGE_NAME);
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * object_open:
 *
 * @retain: if true, do not disrupt the current element such that the
 * opening of the object will be invisible to the state machine, but
 * will still produce the required output.
 *
 * Note: @retain is only meaningful for OUTPUT_FORMAT_JSON.
 *
 * Handle opening an object.
 **/
void
object_open (int retain)
{
	common_assert ();

	if (output_format == OUTPUT_FORMAT_JSON) {
		if (retain) {
			format_element ();
		} else {
			change_element (ELEMENT_TYPE_OBJECT_OPEN);
		}
	} else {
		/* Objects are only required for handling JSON.  In
		 * fact, they cause problems for other output formats
		 * that do not have visible "objects" in that they cause
		 * the state table to lose track of the previous element
		 * since it is actually the previous-previous element
		 * (as the pointless object is the previous element).
		 *
		 * As such, ignore them.
		 */
	}

	switch (output_format) {

	case OUTPUT_FORMAT_CRUMB: /* FALL */
	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_JSON:
		wappend (&doc, L"{");
		break;

	case OUTPUT_FORMAT_XML:
		/* NOP */
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * object_close:
 *
 * @retain: if true, do not disrupt the current element such that the
 * object closure will be invisible to the state machine, but will still
 * produce @retain: if true, do not disrupt the current element.
 *
 * Note: @retain is only meaningful for OUTPUT_FORMAT_JSON.
 *
 * Handle closing an object.
 **/
void
object_close (int retain)
{
	common_assert ();

	if (output_format == OUTPUT_FORMAT_JSON) {
		if (retain) {
			format_element ();
		} else {
			change_element (ELEMENT_TYPE_OBJECT_CLOSE);
		}
	} else {
		/* Objects are only required for handling JSON.  In
		 * fact, they cause problems for other output formats
		 * that do not have visible "objects" in that they cause
		 * the state table to lose track of the previous element
		 * since it is actually the previous-previous element
		 * (as the pointless object is the previous element).
		 *
		 * As such, ignore them.
		 */
	}

	switch (output_format) {

	case OUTPUT_FORMAT_CRUMB: /* FALL */
	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_JSON:
		wappend (&doc, L"}");
		break;

	case OUTPUT_FORMAT_XML:
		/* NOP */
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * section_open:
 *
 * @name: name of section.
 *
 * Start a new section which will contain >0 entry() calls.
 **/
void
section_open (const char *name)
{
	assert (name);
	common_assert ();

	change_element (ELEMENT_TYPE_SECTION_OPEN);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		wappendf (&doc, L"%s:", name);
		break;

	case OUTPUT_FORMAT_CRUMB:
		add_breadcrumb (name);
		break;

	case OUTPUT_FORMAT_JSON:
		wappendf (&doc, L"\"%s\" : {", name);
		break;

	case OUTPUT_FORMAT_XML:
		wappendf (&doc, L"<section name=\"%s\">", name);
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
section_close (void)
{
	common_assert ();

	change_element (ELEMENT_TYPE_SECTION_CLOSE);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_CRUMB:
		remove_breadcrumb ();
		break;

	case OUTPUT_FORMAT_JSON:
		wappend (&doc, L"}");
		break;

	case OUTPUT_FORMAT_XML:
		wappend (&doc, L"</section>");
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * container_open:
 *
 * @name: name of container.
 *
 * Start a new container which will contain >0 entry() calls.
 *
 * This is primarily to handle JSON arrays.
 **/
void
container_open (const char *name)
{
	assert (name);
	common_assert ();

	change_element (ELEMENT_TYPE_CONTAINER_OPEN);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		wappendf (&doc, L"%s:", name);
		break;

	case OUTPUT_FORMAT_CRUMB:
		add_breadcrumb (name);
		break;

	case OUTPUT_FORMAT_JSON:
		wappendf (&doc, L"\"%s\" : [", name);
		break;

	case OUTPUT_FORMAT_XML:
		wappendf (&doc, L"<container name=\"%s\">", name);
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * container_close:
 *
 * Finish with a container.
 **/
void
container_close (void)
{
	common_assert ();

	change_element (ELEMENT_TYPE_CONTAINER_CLOSE);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_CRUMB:
		remove_breadcrumb ();
		break;

	case OUTPUT_FORMAT_JSON:
		wappend (&doc, L"]");
		break;

	case OUTPUT_FORMAT_XML:
		wappend (&doc, L"</container>");
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * _show:
 *
 * @prefix: string prefix to write,
 * @indent: number of spaces to indent output to,
 * @fmt: printf-style format with associated arguments that comprises
 *  the value part.
 *
 * Write output to @string, indented by @indent spaces. A trailing newline
 * will be added.
 *
 * Note that error scenarios cannot call die() as by definition output
 * may not be possible.
 **/
void
_show (const char *prefix, int indent, const char *fmt, ...)
{
	va_list   ap;
	char     *buffer = NULL;

	assert (fmt);

	if (indent)
		appendf (&buffer, "%*s", indent, indent_char);

	if (prefix && *prefix)
		appendf (&buffer, "%s: ", prefix);

	va_start (ap, fmt);
	appendva (&buffer, fmt, ap);
	va_end (ap);

	append (&buffer, "\n");

	_show_output (buffer);

	free (buffer);
}

/**
 * entry:
 *
 * @name: name of thing to display,
 * @fmt: printf-style format with associated arguments that comprises
 *  the value part.
 *
 * Add name/value pair represented by @name and value comprising
 * printf-format string to the @doc global. The value added will be
 * indented appropriately.
 **/
void
entry (const char *name, const char *fmt, ...)
{
	pstring  *encoded_name = NULL;
	pstring  *encoded_value = NULL;
	va_list   ap;

	assert (name);
	assert (fmt);

	common_assert ();

	change_element (ELEMENT_TYPE_ENTRY);

	encoded_name = char_to_pstring (name);
	if (! encoded_name)
		die ("failed to encode name");

	/* annoyingly, we must encode here; we cannot simply call
	 * encode_string() once just prior to showing the output
	 * document since if the output format is XML, we'd end
	 * up encoding the XML tags themselves, not just the values
	 * within!
	 */
	if (encode_string (&encoded_name) < 0)
		die ("failed to encode name");

	/* expand format */
	va_start (ap, fmt);
	wmappendva (&encoded_value, fmt, ap);
	va_end (ap);

	if (encode_string (&encoded_value) < 0)
		die ("failed to encode value");

	switch (output_format) {

	case OUTPUT_FORMAT_CRUMB:
		assert (crumb_list);

		/* Add the bread crumbs */
		PR_LIST_FOREACH (crumb_list, iter) {
			char *crumb = (char *)iter->data;
			wappendf (&doc,
					L"%s%s",
					crumb,
					crumb_separator);
		}

		wappendf (&doc,
				L"%ls%s%ls\n",
				encoded_name->buf,
				text_separator,
				encoded_value->buf);
		break;

	case OUTPUT_FORMAT_TEXT:
		wappendf (&doc,
				L"%ls%s%ls",
				encoded_name->buf,
				text_separator,
				encoded_value->buf);
		break;

	case OUTPUT_FORMAT_JSON:
		wappendf (&doc,
				L"\"%ls\" : \"%ls\"",
				encoded_name->buf,
				encoded_value->buf);
		break;

	case OUTPUT_FORMAT_XML:
		wappendf (&doc,
				L"<entry name=\"%ls\">%ls</entry>",
				encoded_name->buf,
				encoded_value->buf);
		break;

	default:
		assert_not_reached ();
		break;
	}

	pstring_free (encoded_name);
	pstring_free (encoded_value);
}

/**
 * _show_output:
 *
 * @string: String to display.
 *
 * Write output @string to appropriate location based on Output
 * destination.
 **/
void
_show_output_pstring (const pstring *pstr)
{
	char *str;

	assert (pstr);

	str = pstring_to_char (pstr);

	_show_output (str);

	free (str);
}

void
_show_output (const char *str)
{
	int ret;

	assert (str);

	switch (output) {
	case OUTPUT_SYSLOG:
		syslog (LOG_INFO, "%s", str);
		ret = 0;
		break;

	case OUTPUT_STDOUT:
		ret = fputs (str, stdout);
		break;

	case OUTPUT_STDERR:
		ret = fputs (str, stderr);
		break;

	case OUTPUT_TERM:
		assert (user.tty_fd != -1);
		ret = write (user.tty_fd, str, strlen (str));
		if (ret < 0) {
			fprintf (stderr, "ERROR: failed to write to terminal: %s\n",
					strerror (errno));
			exit (EXIT_FAILURE);
		}
		break;

	case OUTPUT_FILE:
		{
			int flags = (O_WRONLY | O_CREAT);

			if (output_file_append)
				flags |= O_APPEND;

			assert (output_file);
			if (output_fd < 0) {
				output_fd = open (output_file, flags,
						(S_IRWXU|S_IRGRP|S_IROTH));
				if (output_fd < 0) {
					fprintf (stderr, "ERROR: failed to open file '%s': %s\n",
							output_file, strerror (errno));
					exit (EXIT_FAILURE);
				}
			}
			ret = write (output_fd, str, strlen (str));
			if (ret < 0) {
				fprintf (stderr, "ERROR: failed to write to file '%s': %s\n",
						output_file, strerror (errno));
				exit (EXIT_FAILURE);
			}
		}
		break;

	default:
		fprintf (stderr, "ERROR: invalid output type: %d\n", output);
		exit (EXIT_FAILURE);
		break;
	}

	if (ret < 0) {
		fprintf (stderr, "ERROR: failed to output message\n");
		exit (EXIT_FAILURE);
	}
}

void
set_indent_amount (int amount)
{
	indent_amount = amount;
}

int
get_indent_amount (void)
{
	return indent_amount;
}


const char *
get_text_separator (void)
{
	return text_separator;
}

void
set_text_separator (const char *s)
{
	text_separator = s;
}

void
set_indent_char (const char *c)
{
	if (! c)
		return;

	// FIXME: free on shutdown !
	indent_char = strdup (c);
}

const char *
get_indent_char (void)
{
	return indent_char;
}

void
set_crumb_separator (const char *c)
{
	if (! c)
		return;

	crumb_separator = strdup (c);
}

void
set_output_file (const char *f)
{
	if (! f)
		return;

	output_file = f;
}

void
set_output_file_append (void)
{
	output_file_append = true;
}

const char *
get_crumb_separator (void)
{
	return crumb_separator;
}

void
reset_indent (void)
{
	indent = 0;
}

/**
 * inc_indent:
 *
 * Increase indent.
 **/
void
inc_indent (void)
{
	assert (indent >= 0);

	indent += indent_amount;
}

/**
 * dec_indent:
 *
 * Decrease indent.
 **/
void
dec_indent (void)
{
	assert (indent >= 0);

	indent -= indent_amount;

	assert (indent >= 0);
}

/**
 * add_indent:
 *
 * Insert the current indent to the output document.
 **/
void
add_indent (pstring **doc)
{
	common_assert ();

	if (! indent)
		return;

	if (! strcmp (indent_char, DEFAULT_INDENT_CHAR)) {
		wappendf (doc, L"%*s", indent, indent_char);
	} else {
		pstring *buffer = NULL;

		// Expand the buffer to the appropriate
		// length by filling it with spaces and a random
		// character.
		wappendf (&buffer, L"%*lc", indent, wide_indent_char);


		/* Now, replace the spaces and the random character with
		 * the chosen character. This convoluted approach is
		 * necessary as printf-type functions don't allow the
		 * padding character to be specified.
		 */
		wmemset (buffer->buf, wide_indent_char, wcslen (buffer->buf));

		pappend (doc, buffer);
		pstring_free (buffer);
	}
}

/**
 * master_header:
 *
 * @doc: document to write footer to.
 *
 * Main header which is displayed once.
 **/
void
master_header (pstring **doc)
{
	common_assert ();

	switch (output_format) {

	case OUTPUT_FORMAT_CRUMB: /* FALL */
	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_JSON:
		object_open (false);
		break;

	case OUTPUT_FORMAT_XML:
		wappend (doc, L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		wappendf (doc, L"<%s version=\"%s\" package_string=\"%s\" "
				"mode=\"%s%s\" format_version=\"%d\">\n",
				PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_STRING,
				user.euid ? _(NON_STR) "-" : "",
				PRIVILEGED_STR,
				PROCENV_FORMAT_VERSION);

		inc_indent ();

		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
add_breadcrumb (const char *name)
{
	assert (name);

	if (! crumb_list)
		crumb_list = pr_list_new (NULL);

	assert (crumb_list);

	pr_list_prepend_str (crumb_list, name);
}

void
remove_breadcrumb (void)
{
	PRList  *entry;

	assert (crumb_list);

	entry = pr_list_remove (crumb_list->prev);
	assert (entry);

	free ((char *)entry->data);
	free (entry);
}

static void
clear_breadcrumbs (void)
{
	assert (crumb_list);

	while (crumb_list->prev != crumb_list)
		remove_breadcrumb ();
}

static void
free_breadcrumbs (void)
{
	if (output_format == OUTPUT_FORMAT_CRUMB && crumb_list) {
		clear_breadcrumbs ();
		free (crumb_list);
	}
}

/**
 * change_element:
 *
 * Handle changing to a new element type. Depending on the output
 * format, this may require separators and newlines to be emitted to
 * produce well-formatted output.
 **/
void
change_element (ElementType new)
{
	common_assert ();

	last_element = current_element;

	current_element = new;

	format_element ();
}

void
format_element (void)
{
	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		format_text_element ();
		break;

	case OUTPUT_FORMAT_CRUMB:
		/* NOP */
		break;

	case OUTPUT_FORMAT_JSON:
		format_json_element ();
		break;

	case OUTPUT_FORMAT_XML:
		format_xml_element ();
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
format_text_element (void)
{
	common_assert ();
	switch (last_element) {

	case ELEMENT_TYPE_ENTRY:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN:
				wappend (&doc, L"\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
				wappend (&doc, L"\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_NONE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
format_json_element (void)
{
	common_assert ();

	switch (last_element) {

	case ELEMENT_TYPE_ENTRY:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L",\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE:
			case ELEMENT_TYPE_OBJECT_OPEN:
				/* NOP */
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				assert_not_reached ();
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L",\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_OPEN:
				/* NOP */
				break;

			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN:
				wappend (&doc, L"\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_CLOSE:
			case ELEMENT_TYPE_SECTION_CLOSE:
				assert_not_reached ();
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L",\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN:
				wappend (&doc, L"\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_CONTAINER_CLOSE:
			case ELEMENT_TYPE_SECTION_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_OPEN:
			case ELEMENT_TYPE_SECTION_OPEN:
				wappend (&doc, L",\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_NONE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
format_xml_element (void)
{
	common_assert ();

	switch (last_element) {

	case ELEMENT_TYPE_ENTRY:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE:
				wappend (&doc, L"\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				assert_not_reached ();
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_CLOSE:
				wappend (&doc, L"\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE:
				assert_not_reached ();
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				wappend (&doc, L"\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_OPEN:
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				wappend (&doc, L"\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN:
				wappend (&doc, L"\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_CONTAINER_OPEN:
				wappend (&doc, L"\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE:
				/* NOP */
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
			case ELEMENT_TYPE_ENTRY:
				wappend (&doc, L"\n");
				dec_indent ();
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_NONE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
set_output_value (const char *name)
{
	struct procenv_map *p;

	assert (name);

	for (p = output_map; p && p->name; p++) {
		if (! strcmp (name, p->name)) {
			output = p->num;
			return;
		}
	}
	die ("invalid output value: '%s'", name);
}

void
set_output_value_raw (Output o)
{
	output = o;
}

void
set_output_format (const char *name)
{
	struct procenv_map *p;

	assert (name);

	for (p = output_format_map; p && p->name; p++) {
		if (! strcmp (name, p->name)) {
			output_format = p->num;
			return;
		}
	}
	die ("invalid output format value: '%s'", name);
}

const char *
get_output_format_name (void)
{
	struct procenv_map *p;

	for (p = output_format_map; p && p->name; p++) {
		if (output_format == p->num)
			return p->name;
	}

	bug ("invalid output format: %d", output_format);

	/* compiler appeasement */
	return NULL;
}

void
handle_indent_char (void)
{
	size_t      len;

	const char *new = indent_char;

	len = mbsrtowcs (NULL, &new, 0, NULL);
	if (len != 1)
		die ("invalid indent character");

	if (mbsrtowcs (&wide_indent_char, &new, len, NULL) != len)
		die ("failed to convert indent character");
}

void
output_init (void)
{
	if (output == OUTPUT_SYSLOG)
		openlog (PACKAGE_NAME, LOG_CONS | LOG_PID, LOG_USER);
}

void
output_finalise (void)
{
	if (output_fd != -1)
		close (output_fd);

	free_breadcrumbs ();

	if (output == OUTPUT_SYSLOG)
		closelog ();
}

/* Performs simple substitution on the input */
static pstring *
output_translate (const pstring *pstr)
{
	pstring         *result = NULL;
	const wchar_t   *start;
	const wchar_t   *p;
	TranslateTable  *table;
	size_t           i;
	size_t           len;
	size_t           extra;
	size_t           bytes;
	size_t           amount;
	wchar_t          from;

	assert (pstr);
	assert (output_format != OUTPUT_FORMAT_TEXT);
	assert (output_format != OUTPUT_FORMAT_CRUMB);

	/* Find the correct translation table for the chosen output format */
	for (i = 0; i < sizeof (translate_table) / sizeof (translate_table[0]); i++) {
		table = &translate_table[i];
		if (table && table->output_format == output_format)
			break;
	}

	if (! table)
		return NULL;

	len = pstr->len;
	start = pstr->buf;

	/* First, calculate the amount of space needed for the expanded
	 * buffer.
	 */
	extra = 0;
	while (start && *start) {
		for (i = 0; i < TRANSLATE_MAP_ENTRIES; i++) {
			from = table->map[i].from;
			if (*start == from) {
				/* Subtract one to take account of the
				 * pre-existing character we're going to
				 * replace.
				 */
				extra += (wcslen (table->map[i].to) - 1);
			}
		}
		start++;
	}

	if (! extra) {
		/* No translation required.
		 *
		 * FIXME: this is inefficient - we should really have
		 * the function accept a 'pstring **' to avoid
		 * re-copying.
		 */
		return pstring_create (pstr->buf);
	}

	len += extra;

	result = pstring_new ();
	if (! result)
		return NULL;

	/* Note that this includes the space for the terminator
	 * (since a pstring's len includes the terminator)
	 */
	bytes = len * sizeof (wchar_t);

	result->buf = malloc (bytes);
	if (! result->buf) {
		pstring_free (result);
		return NULL;
	}

	/* We're using wcsncat() so we'd better make sure there is a
	 * nul for it to find!
	 *
	 * Note: we could have used calloc to do this for us, but
	 * the code is clearer using the @bytes idiom.
	 */
	memset (result->buf, '\0', bytes);

	result->size = bytes;

	/* Sanity check for upcoming overrun check */
	assert (result->buf[len-1] == L'\0');

	/* Now, iterate the string again, performing the actual
	 * replacements.
	 */
	p = start = pstr->buf;

	while (p && *p) {
		for (i = 0; i < TRANSLATE_MAP_ENTRIES; i++) {
			wchar_t  *to;
			size_t    len;

			from = table->map[i].from;

			if (*p != from)
				continue;

			to = table->map[i].to;

			amount = p - start;

			/* Copy from start to match */
			wcsncat (result->buf + result->len, start, amount);

			result->len += amount;

			/* Copy replacement text */
			len = wcslen (to);
			wcsncat (result->buf + result->len, to, len);
			result->len += len;

			/* Jump over the matching character */
			start = p + 1;

			break;
		}
		p++;
	}

	/* Copy remaining non-matching chars */
	amount = p - start;
	wcsncat (result->buf + result->len, start, amount);
	result->len += amount;

	/* Account for terminator */
	result->len += 1;

	/* check for buffer overrun */
	assert (result->buf[len-1] == L'\0');

	return result;
}

/**
 * encode_string:
 *
 * @str: string to encode.
 *
 * Returns: 0 on success, -1 on failure.
 *
 * Convert the specified string to its encoded form. If no encoding is
 * necessary, the string will not be modified.
 *
 * Notes:
 *
 * - By encoding, we mean replacing literals with their
 *   format-langage-specific encodings. For example for XML output,
 *   '<' is converted to '&lt;'.
 *
 * - It is the callers responsibility to free @str iff this function
 *   is successful. any previous value of @str will be freed by
 *   encode_string().
 *
 * BUGS: this is just horribly, horribly gross :(
 **/
int
encode_string (pstring **pstr)
{
	int       ret = 0;
	pstring  *new = NULL;
	wchar_t  *p, *q;
	size_t    non_printables;
	size_t    len = 0;
	size_t    bytes;

	assert (pstr);
	assert (*pstr);

	if ((*pstr)->len <= 1) {
		/* Nothing to do */
		return 0;
	}

	switch (output_format) {

	case OUTPUT_FORMAT_CRUMB: /* FALL */
	case OUTPUT_FORMAT_TEXT:
		/* Nothing to do */
		ret = 0;
		break;

	case OUTPUT_FORMAT_JSON: /* FALL THROUGH */
	case OUTPUT_FORMAT_XML:
		new = output_translate (*pstr);
		if (new) {
			pstring_free (*pstr);
			*pstr = new;
			new = NULL;
		} else {
			ret = -1;
		}
		break;

	default:
		assert_not_reached ();
		break;
	}

	if (ret < 0)
		return ret;

	/* Now, search for evil non-printable characters and encode those
	 * appropriately.
	 */
	for (p = (*pstr)->buf, non_printables = 0; p && *p; p++) {
		if (! iswprint (*p))
			non_printables++;
	}

	if (non_printables &&
			(output_format == OUTPUT_FORMAT_XML
			 || output_format == OUTPUT_FORMAT_JSON)) {

		size_t   new_size = 0;

		wchar_t  *json_format = L"\\u%4.4x";

		/* XXX:
		 *
		 * Although this format spec _may_ produce valid XML,
		 * the rules are arcane and some(?) control characters
		 * cannot be used within an XML document, hence the
		 * "may".
		 *
		 * Aside from simply discarding non-printable characters
		 * (thus distorting the output), we are left with
		 * attempting to produce some sort of encoded
		 * representation which may well choke a validating
		 * parser.
		 *
		 * Realistically, the problem is confined to handling
		 * control characters set in environment variables when
		 * attempting to output XML. This may occur if you run
		 * GNU Screen since it sets $TERMCAP which includes
		 * binary characters.
		 *
		 * FIXME:
		 *
		 * If you hit this issue, raise a bug so we can consider
		 * simply discarding all non-printables when attempting
		 * XML output.
		 */
		wchar_t    *xml_format = L"&#x%2.2x;";

		len = (*pstr)->len;

		/* Calculate expanded size of string by removing
		 * count of non-printable byte and adding back the
		 * number of bytes required to encode them in expanded
		 * form.
		 */
		switch (output_format) {
		case OUTPUT_FORMAT_XML:
			new_size = (len - non_printables) + (non_printables * wcslen (L"&#x..;"));
			break;

		case OUTPUT_FORMAT_JSON:
			new_size = (len - non_printables) + (non_printables * wcslen (L"\\u...."));
			break;
		default:
			break;
		}

		new = pstring_new ();
		if (! new)
			return -1;

		bytes = (1 + new_size) * sizeof (wchar_t);

		new->buf = malloc (bytes);
		if (! new->buf) {
			free (new);
			return -1;
		}

		new->size = bytes;

		memset (new->buf, '\0', bytes);

		for (p = (*pstr)->buf, q = new->buf; p && *p; p++) {
			if (iswprint (*p)) {
				*q = *p;
				q++;
				new->len++;
			} else {
				ret = swprintf (q,
						new_size,
						output_format == OUTPUT_FORMAT_JSON
						? json_format : xml_format,
						*p);
				q += ret;
			}
		}

		/* include terminator */
		new->len = wcslen (new->buf) + 1;

		pstring_free (*pstr);
		*pstr = new;
	}

	return ret;
}
