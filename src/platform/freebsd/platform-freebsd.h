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

#ifndef _PROCENV_PLATFORM_FREEBSD_H
#define _PROCENV_PLATFORM_FREEBSD_H

#include "platform.h"
#include "util.h"

#if defined (HAVE_SYS_CAPABILITY_H)
#include <sys/capability.h>

#if __FreeBSD__ == 9

/* FreeBSD 9 introduced optional capabilities. FreeBSD enabled them by
 * default, changing some of the system calls in the process, so handle
 * the name changes.
 */
#define cap_rights_get(fd, rightsp) cap_getrights (fd, (rightsp))
#define cap_rights_is_set(rightsp, cap) ((*rightsp) & (cap))

#endif /* __FreeBSD__ == 9 */

#define show_capsicum_cap(rights, cap) \
	entry (#cap, "%s", cap_rights_is_set ((&rights), cap) ? YES_STR : NO_STR)
#endif /* HAVE_SYS_CAPABILITY_H */

#endif /* _PROCENV_PLATFORM_FREEBSD_H */
