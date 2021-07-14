/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_MESSAGES_H
#define _PROCENV_MESSAGES_H

/* FIXME: gettext */
#define _(str) str

#define YES_STR                      _("yes")
#define NO_STR                       _("no")
#define NON_STR                      _("non")
#define NA_STR                       _("n/a")
#define UNKNOWN_STR                  _("unknown")
#define MAX_STR                      _(" (max)")
#define DEFINED_STR                  _("defined")
#define NOT_DEFINED_STR              _("not defined")
#define NOT_IMPLEMENTED_STR          _("[not implemented]")
#define BIG_STR                      _("big")
#define LITTLE_STR                   _("little")
#define PRIVILEGED_STR               _("privileged")
#define SUPPRESSED_STR               _("[suppressed]")
#define BUILD_TYPE_STD_STR           _("standard")
#define BUILD_TYPE_REPRODUCIBLE_STR  _("reproducible")

#endif /* _PROCENV_MESSAGES_H */
