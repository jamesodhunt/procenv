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

#ifndef _PROCENV_PLATFORM_LINUX_H
#define _PROCENV_PLATFORM_LINUX_H

/* required for sched_getcpu() (from sched.h) */
#define _GNU_SOURCE

#include "platform.h"
#include "util.h"

#define ROOT_PATH                    "/proc/self/root"

/* Maximum length of a process name as shown by ps(1),
 * /proc/$pid/comm, etc. Requires since there appears to be
 * no standard userspace define for the TASK_COMM_LEN
 * kernel define.
 *
 * See:
 *
 * - PR_SET_NAME in prctl(2).
 * - pthread_setname_np(3).
 */
#define _PROCENV_TASK_COMM_LEN          16

/* The usable amount of space for a process name
 * (1 byte required for terminating '\0')
 */
#define PROCENV_TASK_COMM_NAME_LEN     (_PROCENV_TASK_COMM_LEN-1)

#if defined (HAVE_NUMA_H)

#include <numa.h>
#include <numaif.h>

#if LIBNUMA_API_VERSION == 2
#define PROCENV_NUMA_BITMASK_ISSET(mask, node)	numa_bitmask_isbitset ((mask), (node))
#else
#define PROCENV_NUMA_BITMASK_ISSET(mask, node)	nodemask_isset (&(mask), (node))
#endif
#endif /* HAVE_NUMA_H */

/**
 * LINUX_KERNEL_M:
 * @major: Linux major kernel version number.
 *
 * Returns: true if running Linux kernel is atleast at version
 * specified by @major else false.
 **/
#define LINUX_KERNEL_M(major) \
    (linux_kernel_version (major, -1, -1))

/**
 * LINUX_KERNEL_MM:
 * @major: Linux major kernel version number,
 * @minor: Linux minor kernel version number.
 *
 * Returns: true if running Linux kernel is atleast at version
 * specified by (@major, @minor) else false.
 **/
#define LINUX_KERNEL_MM(major, minor) \
    (linux_kernel_version (major, minor, -1))

/**
 * LINUX_KERNEL_MMR:
 * @major: Linux major kernel version number,
 * @minor: Linux minor kernel version number,
 * @revision: kernel revision version.
 *
 * Returns: true if running Linux kernel is atleast at version
 * specified by (@major, @minor, @revision) else false.
 **/
#define LINUX_KERNEL_MMR(major, minor, revision) \
    (linux_kernel_version (major, minor, revision))

/**
 * show_capability:
 * @caps: cap_t,
 * @cap: capability.
 *
 * Display specified capability, or NOT_DEFINED_STR if value is
 * unknown.
 **/
#ifdef PR_CAPBSET_READ
#define show_capability(caps, cap) \
	_show_capability (caps, cap, #cap)

#define _show_capability(caps, cap, name) \
{ \
	int bound; \
	int effective; \
	int inheritable; \
	int permitted; \
	int ambient; \
	\
	bound = cap_get_bound (cap); \
	\
	ambient = get_ambient_capability (cap); \
	effective = get_capability_by_flag_type (caps, CAP_EFFECTIVE, cap); \
	inheritable = get_capability_by_flag_type (caps, CAP_INHERITABLE, cap); \
	permitted = get_capability_by_flag_type (caps, CAP_PERMITTED, cap); \
	\
	section_open (name); \
	\
	entry ("number", "%d", cap); \
	\
	entry ("supported", "%s", \
			CAP_IS_SUPPORTED (cap) \
			? YES_STR : NO_STR); \
	\
	entry ("in bounding set", "%s", \
			bound < 0 \
			? UNKNOWN_STR \
			: bound \
			? YES_STR \
			: NO_STR); \
	\
	entry ("ambient", "%s", \
			ambient < 0 \
			? NOT_DEFINED_STR \
			: permitted == CAP_SET \
			? YES_STR \
			: NO_STR); \
	\
	entry ("effective", "%s", \
			effective < 0 \
			? NOT_DEFINED_STR \
			: effective == CAP_SET \
			? YES_STR \
			: NO_STR); \
	\
	entry ("inheritable", "%s", \
			inheritable < 0 \
			? NOT_DEFINED_STR \
			: inheritable == CAP_SET \
			? YES_STR \
			: NO_STR); \
	\
	entry ("permitted", "%s", \
			permitted < 0 \
			? NOT_DEFINED_STR \
			: permitted == CAP_SET \
			? YES_STR \
			: NO_STR); \
	\
	section_close (); \
}
#else
#define show_capability(caps, cap)
#endif

#if defined (HAVE_SYS_CAPABILITY_H)

#ifndef CAP_IS_SUPPORTED

static int cap_get_bound (cap_value_t cap)
    __attribute__((unused));

#define CAP_IS_SUPPORTED(cap) (cap_get_bound (cap) >= 0)
#define PROCENV_NEED_LOCAL_CAP_GET_BOUND

#endif /* CAP_IS_SUPPORTED */

#endif /* HAVE_SYS_CAPABILITY_H */

/* semctl(2) states that POSIX.1-2001 requires the caller define this! */
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};

static bool linux_kernel_version (int major, int minor, int revision);

#if defined (HAVE_SYS_CAPABILITY_H)
static int get_capability_by_flag_type (cap_t cap_p, cap_flag_t type, cap_value_t cap)
    __attribute__((unused));
static int get_ambient_capability(cap_value_t cap)
    __attribute__((unused));

#endif /* HAVE_SYS_CAPABILITY_H */

#endif /* _PROCENV_PLATFORM_LINUX_H */
