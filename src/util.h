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

#endif /* _PROCENV_UTIL_H */
