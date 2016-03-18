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

#ifndef _PROCENV_PLATFORM_H
#define _PROCENV_PLATFORM_H

#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <termios.h>
#include <pwd.h>

#ifndef PATH_MAX
/* Hurd. Grrrr.... */
#define PATH_MAX _POSIX_PATH_MAX
#endif

#define PROCENV_SET_DRIVER(_name) \
	{ .name = #_name, .file = __FILE__, }

#if defined (PROCENV_PLATFORM_FREEBSD)
#include <sys/user.h>
#define PROCENV_PR_GET_NAME_LEN (COMMLEN+1)
#else
#define PROCENV_PR_GET_NAME_LEN 16
#endif

extern struct procenv_user user;
extern struct procenv_misc misc;
extern struct procenv_priority priority_io;
extern struct utsname uts;

typedef enum {
	SHOW_ALL,
	SHOW_MOUNTS,
	SHOW_PATHCONF
} ShowMountType;

#include <procenv.h>
#include "platform-generic.h"
#include "platform-headers.h"

struct procenv_priority {
	int process;
	int pgrp;
	int user;
};

struct procenv_user {
	pid_t pid;
	pid_t ppid;
	pid_t sid;

	char proc_name[PROCENV_PR_GET_NAME_LEN];

	pid_t pgroup;
	pid_t fg_pgroup;
	char  ctrl_terminal[L_ctermid];
	int   tty_fd;

	uid_t uid;
	uid_t euid;
	uid_t suid;

	char *login;

	gid_t gid;
	gid_t egid;
	gid_t sgid;

	struct passwd passwd;
};

struct procenv_misc {
	char   cwd[PATH_MAX];
	char   root[PATH_MAX];
	mode_t umask_value;
	int cpu;
#if defined (PROCENV_PLATFORM_FREEBSD)
	int    in_jail;
#endif
};

struct procenv_driver
{
	const char *name;
	const char *file;
};

/*
 * - get_*() functions obtain information.
 * - show_*() functions display entries for a particular category of
 *   information.
 * - handle_*() fuctions are similar to show_*() ones, except that they
 *   also emit the appropriate heading/section/container entries and
 *   corresponding footers.
 */
struct procenv_ops
{
	struct procenv_driver driver;

	void (*init) (void);
	void (*cleanup) (void);

	const struct procenv_map *signal_map;
	const struct procenv_map *if_flag_map;
	const struct procenv_map *personality_map;
	const struct procenv_map *personality_flag_map;

	void (*get_user_misc) (struct procenv_user *user,
			       struct procenv_misc *misc);

	void (*get_proc_name) (struct procenv_user *user);

	void (*get_io_priorities) (struct procenv_priority *iop);
	void (*get_tty_locked_status) (struct termios *lock_status);

	long (*get_kernel_bits) (void);
	int (*get_mtu) (const struct ifaddrs *ifaddr);

	void (*show_capabilities) (void);
	void (*show_cgroups) (void);
	void (*show_confstrs) (void);
	void (*show_cpu_affinities) (void);
	void (*show_cpu) (void);
	void (*show_extended_if_flags) (const char *interface,
					unsigned short *flags);
	void (*show_fd_capabilities) (int fd);
	void (*show_fds) (void);
	void (*show_io_priorities) (void);
	void (*show_mounts) (ShowMountType what);
	void (*show_msg_queues) (void);
	void (*show_namespaces) (void);
	void (*show_oom) (void);
	void (*show_prctl) (void);
	void (*show_rlimits) (void);
	void (*show_security_module) (void);
	void (*show_semaphores) (void);
	void (*show_shared_mem) (void);
	void (*show_timezone) (void);

	void (*handle_numa_memory) (void);
	void (*handle_proc_branch) (void);
	void (*handle_scheduler_type) (void);

	PROCENV_CPU_SET_TYPE *(*get_cpuset) (void);
	void (*free_cpuset) (PROCENV_CPU_SET_TYPE *cs);
	bool (*cpuset_has_cpu) (const PROCENV_CPU_SET_TYPE *cs,
			PROCENV_CPU_TYPE cpu);
};

#endif /* _PROCENV_PLATFORM_H */
