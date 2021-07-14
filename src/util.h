/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_UTIL_H
#define _PROCENV_UTIL_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#if defined (PROCENV_PLATFORM_LINUX)
#include <linux/vt.h>
#endif /* PROCENV_PLATFORM_LINUX */

#if defined (PROCENV_PLATFORM_FREEBSD)
#include <sys/consio.h>
#endif

#include "platform.h"

bool has_ctty (void);
bool in_chroot (void);
bool is_big_endian (void);
bool is_process_group_leader (void);
bool is_session_leader (void);
bool uid_match (uid_t uid);
const char *container_type (void);
const char *get_speed (speed_t speed);
int fd_valid (int fd);

#if !defined (PROCENV_PLATFORM_HURD) && \
    !defined (PROCENV_PLATFORM_MINIX) && \
    !defined (PROCENV_PLATFORM_DARWIN)
int is_console (int fd);
#endif

#if !defined (PROCENV_PLATFORM_HURD)

#define mk_mem_section(name, value) \
{ \
	section_open (name); \
	entry ("bytes", "%lu", value); \
	show_human_size_entry (value); \
	section_close (); \
}

#endif /* !PROCENV_PLATFORM_HURD */

#endif /* _PROCENV_UTIL_H */
