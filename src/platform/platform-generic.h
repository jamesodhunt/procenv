/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_PLATFORM_GENERIC_H
#define _PROCENV_PLATFORM_GENERIC_H

#include "types.h"

void show_fds_generic (void);
void show_rlimits_generic (void);
void show_confstrs_generic (void);
long get_kernel_bits_generic (void);
void show_human_size_entry (size_t value);

#if !defined (PROCENV_PLATFORM_MINIX)
int get_mtu_generic (const struct ifaddrs *ifaddr);
#endif

#if defined (PROCENV_PLATFORM_LINUX) || \
    defined (PROCENV_PLATFORM_BSD)   || \
    defined (PROCENV_PLATFORM_HURD)  || \
    defined (PROCENV_PLATFORM_MINIX)

void show_cpu_affinities_generic (void);

#endif

#if defined (PROCENV_PLATFORM_LINUX) || \
    defined (PROCENV_PLATFORM_BSD)   || \
    defined (PROCENV_PLATFORM_HURD)  || \
    defined (PROCENV_PLATFORM_MINIX) || \
    defined (PROCENV_PLATFORM_DARWIN)

void show_pathconfs (ShowMountType what, const char *dir);

#endif

#if defined (PROCENV_PLATFORM_BSD)   || \
    defined (PROCENV_PLATFORM_MINIX) || \
    defined (PROCENV_PLATFORM_DARWIN)

char *get_mount_opts_generic_bsd (const struct procenv_map64 *opts, uint64_t flags);
void show_mounts_generic_bsd (ShowMountType what,
		const struct procenv_map64 *mntopt_map);

#endif

#if defined (PROCENV_PLATFORM_LINUX) || defined (PROCENV_PLATFORM_HURD)

void show_mounts_generic_linux (ShowMountType what);
int get_canonical_generic_linux (const char *path, char *canonical, size_t len);

#endif /* PROCENV_PLATFORM_LINUX || PROCENV_PLATFORM_HURD */

#if !defined (PROCENV_PLATFORM_DARWIN)
bool get_time_generic (struct timespec *ts);
void show_memory_generic (void);
#endif

void show_clocks_generic (void);

#if defined (PROCENV_PLATFORM_LINUX) || defined (PROCENV_PLATFORM_BSD) || defined (PROCENV_PLATFORM_DARWIN)

void show_timezone_generic (void);

#endif

#if ! defined (PROCENV_PLATFORM_ANDROID) && ! defined (PROCENV_PLATFORM_DARWIN)

void show_libs_generic(void);

#endif

#endif /* _PROCENV_PLATFORM_GENERIC_H */
