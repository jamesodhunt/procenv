/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_PLATFORM_DARWIN_H
#define _PROCENV_PLATFORM_DARWIN_H

#include "platform.h"
#include "util.h"

#include <mach/mach.h>
#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <libgen.h>

#define mib_len(mib) ((sizeof (mib) / sizeof(*mib)) - 1)

#define mach_header_size(h) \
    (((h)->magic == MH_MAGIC_64 || (h)->magic == MH_CIGAM_64) \
        ? sizeof(struct mach_header_64) \
        : sizeof(struct mach_header))

#endif /* _PROCENV_PLATFORM_DARWIN_H */
