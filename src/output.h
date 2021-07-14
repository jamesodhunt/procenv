/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_OUTPUT_H
#define _PROCENV_OUTPUT_H

#include "types.h"
#include "pstring.h"

#define show(...) _show ("", get_indent_amount (), __VA_ARGS__)

/**
 * showi:
 *
 * @_indent: additional indent amount,
 * @fmt: printf-style format and optional arguments.
 *
 * Write indented message to appropriate output location.
 **/
#define showi(_indent, ...) \
    _show ("", get_indent_amount() + _indent, __VA_ARGS__)

/**
 * _message:
 * @prefix: Fixed message prefix,
 * @fmt: printf-style format and optional arguments.
 *
 * Write unindented message to appropriate output location.
 **/
#define _message(prefix, ...) _show (prefix, 0, __VA_ARGS__)

#define warn(...) \
{ \
	_message ("WARNING",  __VA_ARGS__); \
}

#ifdef DEBUG
/* for when running under GDB */
#define die_finalise() raise (SIGUSR1)
#else
#define die_finalise() exit (EXIT_FAILURE)
#endif

#define bug(...) \
{ \
	_show ("BUG", 0, __VA_ARGS__); \
	exit (EXIT_FAILURE); \
}

#define POINTER_SIZE (sizeof (void *))

#define die(...) \
{ \
	output = OUTPUT_STDERR; \
	_message ("ERROR", __VA_ARGS__); \
	cleanup (); \
	die_finalise (); \
}

#define common_assert() \
	assert (doc); \
	assert (get_indent_amount() >= 0)

#define assert_not_reached() \
	do { \
		die ("%s:%d: Not reached assertion failed in %s", \
			   __FILE__, __LINE__, __func__); \
	} while (0)

typedef enum procenv_output {
	OUTPUT_FILE,
	OUTPUT_STDERR,
	OUTPUT_STDOUT,
	OUTPUT_SYSLOG,
	OUTPUT_TERM
} Output;

typedef enum {
	OUTPUT_FORMAT_TEXT,
	OUTPUT_FORMAT_CRUMB,
	OUTPUT_FORMAT_JSON,
	OUTPUT_FORMAT_XML
} OutputFormat;

typedef enum element_type {
	ELEMENT_TYPE_ENTRY,
	ELEMENT_TYPE_SECTION_OPEN,
	ELEMENT_TYPE_SECTION_CLOSE,
	ELEMENT_TYPE_CONTAINER_OPEN,
	ELEMENT_TYPE_CONTAINER_CLOSE,
	ELEMENT_TYPE_OBJECT_OPEN,
	ELEMENT_TYPE_OBJECT_CLOSE,
	ELEMENT_TYPE_NONE = -1
} ElementType;

/********************************************************************/

extern Output        output;
extern OutputFormat  output_format;
extern wchar_t       wide_indent_char;

void cleanup (void);

/********************************************************************/

void output_init (void);
void output_finalise (void);

void header (const char *name);
void footer (void);

void master_header (pstring **doc);
void master_footer (pstring **doc);

void object_open (int retain);
void object_close (int retain);

void section_open (const char *name);
void section_close (void);

void container_open (const char *name);
void container_close (void);

void entry (const char *name, const char *fmt, ...);
void _show (const char *prefix, int indent, const char *fmt, ...);
void _show_output (const char *str);
void _show_output_pstring (const pstring *pstr);

void inc_indent (void);
void dec_indent (void);
void reset_indent (void);
void add_indent (pstring **doc);

void set_indent_amount (int amount);
int get_indent_amount (void);
const char *get_indent_char (void);

void set_indent_char (const char *c);
void handle_indent_char (void);

void add_breadcrumb (const char *name);
void remove_breadcrumb (void);
void set_crumb_separator (const char *c);
const char *get_crumb_separator (void);

void set_output_file (const char *f);
void set_output_file_append (void);

void change_element (ElementType new);
void format_element (void);

void format_text_element (void);
void format_json_element (void);
void format_xml_element (void);

void set_output_value (const char *name);
void set_output_value_raw (Output o);
void set_output_format (const char *name);
const char *get_output_format_name (void);

const char *get_text_separator (void);
void set_text_separator (const char *s);

#endif /* _PROCENV_OUTPUT_H */
