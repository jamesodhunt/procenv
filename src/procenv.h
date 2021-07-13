/*--------------------------------------------------------------------
 * Copyright © 2012-2016 James Hunt <jamesodhunt@gmail.com>.
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

#ifndef PROCENV_H
#define PROCENV_H

/* for dl_iterate_phdr(3) */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <wchar.h>
#include <wctype.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <paths.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

/* XXX: must come before sched.h on Minix! */
#include <pthread.h>

#include <sched.h>
#include <termios.h>
#include <getopt.h>
#include <assert.h>
#include <sys/types.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fenv.h>
#include <sys/utsname.h>
#include <locale.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>

#if !defined (PROCENV_PLATFORM_DARWIN)
#include <sys/sysinfo.h>
#endif

#include "util.h"
#include "string-util.h"
#include "pr_list.h"
#include "output.h"
#include "platform.h"

#include "platform-headers.h"

/*********************************************************************/
// FIXME: ANDROID - need to auto-generate values!!

#if defined PROCENV_PLATFORM_ANDROID

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "procenv"
#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "0.24"
#endif

#ifndef PACKAGE_STRING
#define PACKAGE_STRING PACKAGE_NAME
#endif

/* major(3) / minor(3) */
#include <sys/sysmacros.h>

#endif /* PROCENV_PLATFORM_ANDROID */

/*********************************************************************/

/**
 * PROCENV_FORMAT_VERSION:
 *
 * Version of output format.
 *
 * XXX: must be updated for every change.
 *
 * VERSION 1:
 *   - Original format (up to procenv 0.27).
 * VERSION 2:
 *   - Added --memory.
 *   - Expanded --cpu.
 *   - Added missing --cpu to default output.
 *   - Moved memory page size from --misc to --memory.
 * VERSION 3:
 *   - --fds: Added Capsicum capabilities for FreeBSD (alas this version
 *     is a NOP for non-BSD platforms).
 * VERSION 4:
 *   - Added "symbolic" values to --range output.
 * VERSION 5:
 *   - Capabilities output changed on linux to show not just the
 *     bounding set, but also permitted, inheritable and enabled values
 *     along with the numeric value of the define.
 * VERSION 6:
 *   - --misc: Added personality.
 * VERSION 7:
 *   - --confstr and pathconf values in --mount now show
 *     NA_STR rather than UNKNOWN_STR. --sysconf now shows NA_STR rather
 *     than -1.
 * VERSION 8:
 *   - --memory: Added NUMA API version.
 *   - --shared-memory: Added swap_attempts and swap_successes.
 * VERSION 9:
 *   - --meta now includes a 'build-type' field.
 * VERSION 10:
 *   - Output is now sorted correctly (again - previously show_misc()
 *     and show_libc() were being called in the wrong order).
 * VERSION 11:
 *   - Added --namespaces.
 * VERSION 12:
 *   - Change format of --namespaces output.
 * VERSION 13:
 *   - Added I/O priority to --cpu.
 * VERSION 14:
 *   - Added driver to --meta.
 * VERSION 15:
 *   - Renamed "linux security module" to "security" module in --misc.
 *   - Added procenv driver details to --meta.
 * VERSION 16:
 *   - Sort feature-test macros.
 * VERSION 17:
 *   - Added 'vm' to --misc output.
 * VERSION 18:
 *   - Added 'ambient' to --capabilities output.
 * VERSION 19:
 *  - Added more details to --memory and new capabilities to --capabilities.
 * VERSION 20:
 *  - More --memory details for Darwin.
 * VERSION 21:
 *  - Added `CLOCK_BOOTTIME_ALARM`, `CLOCK_PROCESS_CPUTIME_ID`
 *    and `CLOCK_REALTIME_ALARM` to --clocks output and supported --timezone
 *    on Darwin.
 * VERSION 22:
 *  - Supported --libs on Darwin.
 * VERSION 23:
 *  - Changed output order of --sysconf and --confstr, added various new
 *    sysconf values, plus changed output format of --namespaces.
 **/
#define PROCENV_FORMAT_VERSION 23

#if defined (PROCENV_PLATFORM_LINUX) || defined (PROCENV_PLATFORM_HURD)

#if ! defined (_LINUX_CAPABILITY_VERSION_3) && ! defined (CAP_LAST_CAP)
/* Ugh */
#define CAP_LAST_CAP 30
#endif

#endif /* PROCENV_PLATFORM_LINUX || PROCENV_PLATFORM_HURD */

/* Environment Variables */
#define PROCENV_OUTPUT_ENV           "PROCENV_OUTPUT"
#define PROCENV_FORMAT_ENV           "PROCENV_FORMAT"
#define PROCENV_FILE_ENV             "PROCENV_FILE"
#define PROCENV_FILE_APPEND_ENV      "PROCENV_FILE_APPEND"
#define PROCENV_EXEC_ENV             "PROCENV_EXEC"
#define PROCENV_INDENT_ENV           "PROCENV_INDENT"
#define PROCENV_INDENT_CHAR_ENV      "PROCENV_INDENT_CHAR"
#define PROCENV_SEPARATOR_ENV        "PROCENV_SEPARATOR"
#define PROCENV_CRUMB_SEPARATOR_ENV  "PROCENV_CRUMB_SEPARATOR"

#define PROCENV_BUFFER                1024

/* FIXME: explain! */
#define CTIME_BUFFER                  32

#include "messages.h"

/* Size of blocks we will show the user (as df(1) does) */
#define DF_BLOCK_SIZE 1024

#define PROGRAM_AUTHORS "James O. D. Hunt <jamesodhunt@gmail.com>"

#define type_hex_width(type) \
	(sizeof (type) * 2)

#define show_clock_res(clock) \
{ \
	struct timespec res; \
	section_open (#clock); \
	if (clock_getres (clock, &res) < 0) \
		entry ("resolution", "%s", UNKNOWN_STR); \
	else \
		entry ("resolution", "%ld.%09lds", res.tv_sec, res.tv_nsec); \
	section_close (); \
}

#define show_const(t, flag, constant) \
	object_open (false); \
    	entry (#constant, "%d", !!(t.flag & constant)); \
	object_close (false)

/**
 * Show a terminal special characters attribute.
 *
 * t: struct termios,
 * elem: element of c_cc array,
 * lock_status: struct termios representing lock status of @t.
 **/
#define show_cc_tty(t, elem, lock_status) \
    entry (#elem, "0x%x%s", \
	t.c_cc[elem], \
	lock_status.c_cc[elem] ? " (locked)" : "");

/**
 * Show a terminal attribute constant value.
 *
 * t: struct termios,
 * flag: name of attribute,
 * constant: value of @flag,
 * lock_status: struct termios representing lock status of @t.
 **/
#define show_const_tty(t, flag, constant, lock_status) \
	entry (#constant, \
		"%d%s", \
		!!(t.flag & constant), \
		!!(lock_status.flag) ? " (locked)" : "")

#define show_pathconf(what, path, name) \
{ \
	long conf; \
	errno = 0; \
	conf = pathconf (path, name); \
	if (conf == -1 && errno == 0) { \
	    entry (#name, "%s", NA_STR); \
	} else { \
	    entry (#name, "%d", conf); \
	} \
}

#define SPEED(s) \
	{#s, s}

#define mk_map_entry(s) \
	{#s, s}

#define show_confstr(s) \
{ \
    _show_confstr (s, #s); \
}

#define _show_confstr(s, name) \
{ \
	size_t len; \
	char *buffer; \
	\
	errno = 0; \
	len = confstr(s, NULL, 0); \
	if (len && errno == 0) { \
		\
		buffer = calloc (1, len); \
		if (! buffer) { \
			die ("failed to allocate space for confstr"); \
		} \
		\
		assert (confstr (s, buffer, len) == len); \
		\
		/* Convert multi-line values to multi-field */ \
		for (size_t i = 0; i < len; i++) { \
			if (buffer[i] == '\n') buffer[i] = ' '; \
		} \
		\
		entry (name, "%s%s%s", \
				buffer && buffer[0] ? "'" : "", \
				buffer && buffer[0] ? buffer : NA_STR, \
				buffer && buffer[0] ? "'" : ""); \
		\
		free (buffer); \
	} \
}

/* Note: param is ignored */
#define limit_max(l) \
	((unsigned long int)-1)

#define show_limit(limit) \
{ \
	struct rlimit tmp; \
	\
	if (getrlimit (limit, &tmp) < 0) { \
		die ("failed to query rlimit '%s'", #limit); \
	} \
	\
	section_open (#limit); \
	\
	section_open ("soft"); \
	entry ("current", "%lu", (unsigned long int)tmp.rlim_cur); \
	entry ("max", "%lu", limit_max (limit)); \
	section_close (); \
	\
	section_open ("hard"); \
	entry ("current", "%lu", (unsigned long int)tmp.rlim_max); \
	entry ("max", "%lu", limit_max (limit)); \
	section_close (); \
	\
	section_close (); \
}

#define show_usage(rusage, name) \
	entry (#name, "%lu", rusage.name)

#define get_sysconf(s) \
 	sysconf (s)

#define mk_sysconf_map_entry(name) \
	{#name, name }

#define show_sizeof_type(type) \
	entry (#type, "%lu byte%s", \
			(unsigned long int)sizeof (type), \
			sizeof (type) == 1 ? "" : "s")


#define show_size(type) \
	entry ("size", "%lu byte%s", \
			(unsigned long int)sizeof (type), \
			sizeof (type) == 1 ? "" : "s")

#define free_if_set(ptr) \
    if (ptr) free (ptr)

char *get_path (const char *argv0);
char *get_personality_flags (unsigned int flags);
const char *get_arch (void);
const char *get_group_name (gid_t gid);
const char *get_os (void);
const char *get_personality_name (unsigned int domain);
const char *get_signal_name (int signum);
const char *get_thread_scheduler_name (int sched);
const char *get_user_name (gid_t gid);
int get_major_minor (const char *path, unsigned int *_major, unsigned int *_minor);
long get_kernel_bits (void);
void get_priorities (void);
void get_uname (void);

void show_all_groups (void);
void show_capabilities (void);
void show_cgroups (void);
void show_clocks (void);
void show_compiler (void);
void show_confstrs (void);
void show_data_model (void);
void show_libc (void);
void show_libs (void);
void show_locale (void);
void show_mounts (ShowMountType what);
void show_misc (void);
void show_msg_queues (void);
void show_network (void);
void show_oom (void);
void show_ranges (void);
void show_rlimits (void);
void show_semaphores (void);
void show_shared_mem (void);
void show_sizeof (void);
void show_threads (void);
void show_time (void);
void show_timezone (void);
void show_tty_attrs (void);
void show_uname (void);
void show_version (void);

void handle_proc_branch (void);

char *format_perms (mode_t mode);
void format_time (const time_t *t, char *buffer, size_t len);
void restore_locale (void);
int qsort_compar (const void *a, const void *b);

extern char **environ;

extern Output output;
extern pstring *doc;

#endif /* PROCENV_H */
