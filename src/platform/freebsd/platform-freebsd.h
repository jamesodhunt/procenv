/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_PLATFORM_FREEBSD_H
#define _PROCENV_PLATFORM_FREEBSD_H

#include "platform.h"
#include "util.h"

#if defined (HAVE_SYS_CAPSICUM_H)
    #include <sys/capsicum.h>
#else /* !HAVE_SYS_CAPSICUM_H */
    #if defined (HAVE_SYS_CAPABILITY_H)
        #include <sys/capability.h>
    #endif /* HAVE_SYS_CAPABILITY_H */
#endif /* HAVE_SYS_CAPSICUM_H */

#if defined (HAVE_SYS_CAPSICUM_H) || defined (HAVE_SYS_CAPABILITY_H)

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
#endif /* HAVE_SYS_CAPSICUM_H || HAVE_SYS_CAPSICUM_H */

#endif /* _PROCENV_PLATFORM_FREEBSD_H */
