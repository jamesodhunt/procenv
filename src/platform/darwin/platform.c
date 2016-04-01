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

#include "platform-darwin.h"

static struct procenv_map signal_map_darwin[] = {

	mk_map_entry (SIGALRM),
	mk_map_entry (SIGBUS),
	mk_map_entry (SIGCONT),
	mk_map_entry (SIGFPE),
	mk_map_entry (SIGHUP),
	mk_map_entry (SIGILL),
	mk_map_entry (SIGINT),
	mk_map_entry (SIGKILL),
	mk_map_entry (SIGPIPE),
	mk_map_entry (SIGQUIT),
	mk_map_entry (SIGSEGV),
	mk_map_entry (SIGSTOP),
	mk_map_entry (SIGTERM),
	mk_map_entry (SIGTRAP),
	mk_map_entry (SIGTSTP),
	mk_map_entry (SIGTTIN),
	mk_map_entry (SIGTTOU),
	mk_map_entry (SIGUSR1),
	mk_map_entry (SIGUSR2),
	mk_map_entry (SIGXCPU),
	mk_map_entry (SIGXFSZ),
	mk_map_entry (SIGIOT),
	mk_map_entry (SIGPROF),
	mk_map_entry (SIGSYS),
	mk_map_entry (SIGURG),
	mk_map_entry (SIGVTALRM),

#if defined (SIGIO)
	mk_map_entry (SIGIO),
#endif

#if defined (SIGWINCH)
	mk_map_entry (SIGWINCH),
#endif

#if defined (SIGINFO)
	mk_map_entry (SIGINFO),
#endif

#if defined (SIGPOLL)
	{ SIGPOLL, "SIGPOLL|SIGEMT" },
#elif defined (SIGEMT)
	{ SIGEMT, "SIGPOLL|SIGEMT" },
#endif

	{ SIGCHLD, "SIGCHLD|SIGCLD" },
	{ SIGABRT, "SIGABRT|SIGIOT" },

	{ 0, NULL },
};

static struct procenv_map64 mntopt_map_darwin[] = {

	{ MNT_ASYNC        , "asynchronous" },
	{ MNT_EXPORTED     , "NFS-exported" },
	{ MNT_LOCAL        , "local" },
	{ MNT_MULTILABEL   , "multilabel" },
	{ MNT_NOATIME      , "noatime" },
	{ MNT_NOEXEC       , "noexec" },
	{ MNT_NOSUID       , "nosuid" },
	{ MNT_QUOTA        , "with quotas" },
	{ MNT_RDONLY       , "read-only" },
	{ MNT_SYNCHRONOUS  , "synchronous" },
	{ MNT_UNION        , "union" },

	{ 0, NULL }
};

static struct procenv_map if_flag_map_darwin[] = {
	mk_map_entry (IFF_UP),
	mk_map_entry (IFF_BROADCAST),
	mk_map_entry (IFF_DEBUG),
	mk_map_entry (IFF_LOOPBACK),
	mk_map_entry (IFF_POINTOPOINT),
	mk_map_entry (IFF_RUNNING),
	mk_map_entry (IFF_NOARP),
	mk_map_entry (IFF_PROMISC),
	mk_map_entry (IFF_ALLMULTI),
	mk_map_entry (IFF_SIMPLEX),
	mk_map_entry (IFF_MULTICAST),

	{ 0, NULL }
};

static void
show_cpu_darwin (void)
{
	long  max;

	max = get_sysconf (_SC_NPROCESSORS_ONLN);

	/* XXX: possible to determine current cpu? */
	entry ("number", "%s of %lu", UNKNOWN_STR, max);
}

static bool
get_time_darwin (struct timespec *ts)
{
    clock_serv_t     cs;
    mach_timespec_t  mts;

    // FIXME: can this fail?
    host_get_clock_service (mach_host_self(), CALENDAR_CLOCK, &cs);

    // FIXME: can this fail?
    clock_get_time (cs, &mts);

    // FIXME: can this fail?
    mach_port_deallocate (mach_task_self(), cs);

    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;

    return 0;
}

static void
handle_proc_branch_darwin (void)
{
	struct kinfo_proc   *procs = NULL;
	struct kinfo_proc   *p;
	static const int     mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
	int                  ret;
	int                  done = false;
	size_t               bytes;
	size_t               count;
	size_t               i;
	pid_t                current;
	pid_t                ultimate_parent = 0;
	char                *str = NULL;

	/* arbitrary */
	int                  attempts = 20;

	current = getpid ();

	/* XXX: This system interface seems horribly racy - what if the numbe of pids
	 * XXX: changes between the 1st and 2nd call to sysctl() ???
	 */
	while (! done) {
		attempts--;
		if (! attempts)
			return;

		/* determine how much space required to store details of all
		 * processes.
		 */
		ret = sysctl ((int *)mib, mib_len (mib), NULL, &bytes, NULL, 0);
		if (ret < 0)
			return;

		count = bytes / sizeof (struct kinfo_proc);

		/* allocate space */
		procs = calloc (count, sizeof (struct kinfo_proc));
		if (! procs)
			return;

		/* request the details of the processes */
		ret = sysctl ((int *)mib, mib_len (mib), procs, &bytes, NULL, 0);
		if (ret < 0) {
			free (procs);
			procs = NULL;
			if (errno != ENOMEM) {
				/* unknown error, so give up */
				return;
			}
		} else {
			done = true;
		}
	}

	if (! done)
		goto out;

	/* reset */
	done = false;

	/* Calculate the lowest PID number which gives us the ultimate
	 * parent of all processes.
	 */
	p = &procs[0];
	ultimate_parent = p->kp_proc.p_pid;

	for (i = 1; i < count; i++) {
		p = &procs[i];
		if (p->kp_proc.p_pid < ultimate_parent)
			ultimate_parent = p->kp_proc.p_pid;
	}

	while (! done) {
		for (i = 0; i < count && !done; i++) {
			p = &procs[i];

			if (p->kp_proc.p_pid == current) {

				if (! ultimate_parent && current == ultimate_parent) {

					/* Found the "last" PID so record it and hop out */
					appendf (&str, "%d ('%s')",
							(int)current, p->kp_proc.p_comm);

					done = true;
					break;
				} else {
					/* Found a valid parent pid */
					appendf (&str, "%d ('%s'), ",
							(int)current, p->kp_proc.p_comm);
				}

				/* Move on */
				current = p->kp_eproc.e_ppid;
			}
		}
	}

	entry ("ancestry", "%s", str);

out:
	free_if_set (str);
	free_if_set (procs);
}


static void
show_mounts_darwin (ShowMountType what)
{
	show_mounts_generic_bsd (what, mntopt_map_darwin);
}

/* Darwin lacks:
 *
 * - cpusets and cpu affinities.
 *
 */
struct procenv_ops platform_ops =
{
    .driver                        = PROCENV_SET_DRIVER (darwin),

    .signal_map                    = signal_map_darwin,
    .if_flag_map                   = if_flag_map_darwin,

    .get_time                      = get_time_darwin,
    .get_kernel_bits               = get_kernel_bits_generic,
    .get_mtu                       = get_mtu_generic,

    .handle_proc_branch            = handle_proc_branch_darwin,

    .show_confstrs                 = show_confstrs_generic,
    .show_cpu                      = show_cpu_darwin,
    .show_fds                      = show_fds_generic,
    .show_mounts                   = show_mounts_darwin,
    .show_rlimits                  = show_rlimits_generic,
};
