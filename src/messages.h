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
