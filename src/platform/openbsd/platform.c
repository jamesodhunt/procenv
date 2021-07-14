/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#include "platform-openbsd.h"

static struct procenv_map signal_map_openbsd[] = {

    { SIGABRT, "SIGABRT|SIGIOT" },

	mk_map_entry (SIGALRM),
	mk_map_entry (SIGBUS),

	{ "SIGCHLD|SIGCLD", SIGCHLD },

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
	mk_map_entry (SIGUSR1),
	mk_map_entry (SIGUSR2),
	mk_map_entry (SIGIO),
	mk_map_entry (SIGPROF),
	mk_map_entry (SIGSYS),
	mk_map_entry (SIGURG),
	mk_map_entry (SIGVTALRM),
	mk_map_entry (SIGWINCH),
	mk_map_entry (SIGXCPU),
	mk_map_entry (SIGXFSZ),
	mk_map_entry (SIGEMT),
	mk_map_entry (SIGINFO),
	mk_map_entry (SIGTHR),
	mk_map_entry (SIGTTOU),

	{ NULL, 0 },
};

static struct procenv_map64 mntopt_map[] = {

	{ "asynchronous" , MNT_ASYNC },
	{ "NFS-exported" , MNT_EXPORTED },
	{ "local"        , MNT_LOCAL },
	{ "noatime"      , MNT_NOATIME },
	{ "noexec"       , MNT_NOEXEC },
	{ "nosuid"       , MNT_NOSUID },
	{ "with-quotas"  , MNT_QUOTA },
	{ "read-only"    , MNT_RDONLY },
	{ "synchronous"  , MNT_SYNCHRONOUS },

#if defined (MNT_SUJ)
	{ "journaled-soft-updates", MNT_SUJ },
#endif
	{ "synchronous", MNT_SYNCHRONOUS },

	{ NULL, 0 }
};

static struct procenv_map if_flag_map_openbsd[] = {
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

	{ NULL, 0 }
};

static void
show_mounts_openbsd (ShowMountType what)
{
	show_mounts_generic_bsd (what, mntopt_map);
}

/* Who would have thought handling PIDs was so tricky? */
static void
handle_proc_branch_openbsd (void)
{
	int                  count = 0;
	int                  i;
	char                 errors[_POSIX2_LINE_MAX];
	kvm_t               *kvm;
	struct kinfo_proc   *procs;
	struct kinfo_proc   *p;
	pid_t                current;
	int                  done = false;
	char                *str = NULL;
	pid_t                ultimate_parent = 0;

	common_assert ();

	current = getpid ();

	kvm = kvm_openfiles (NULL, NULL, NULL, KVM_NO_FILES, errors);
	if (! kvm)
		die ("unable to open kvm");

	procs = kvm_getprocs (kvm, KERN_PROC_ALL, 0, sizeof (struct kinfo_proc), &count);
	if (! procs)
		die ("failed to get process info");

	/* Calculate the lowest PID number which gives us the ultimate
	 * parent of all processes.
	 *
	 * OpenBSD has init as PID 1.
	 */

	p = &procs[0];
	ultimate_parent = p->p_pid;

	for (i = 1; i < count; i++) {
		p = &procs[i];
		if (p->p_pid< ultimate_parent)
			ultimate_parent = p->p_pid;
	}

	while (! done) {
		for (i = 0; i < count && !done; i++) {
			p = &procs[i];

			if (p->p_pid == current) {
				if (ultimate_parent == 1 && current == ultimate_parent) {
					/* Found the "last" PID so record it and hop out */
					appendf (&str, "%d ('%s')",
							(int)current,
							p->p_comm);
					done = true;
					break;
				} else {
					/* Found a valid parent pid */
					appendf (&str, "%d ('%s'), ",
							(int)current,
							p->p_comm);
				}

				/* Move on */
				current = p->p_ppid;
			}
		}
	}

	if (kvm_close (kvm) < 0)
		die ("failed to close kvm");

	entry ("ancestry", "%s", str);
	free (str);
}

/* XXX:Notes:
 *
 * - show_cpu : it doesn't appear you can query the CPU of the
 *   current process on OpenBSD :-(
 */
struct procenv_ops platform_ops =
{
	.driver                        = PROCENV_SET_DRIVER (openbsd),

	.get_kernel_bits               = get_kernel_bits_generic,
	.get_mtu                       = get_mtu_generic,
	.get_time                      = get_time_generic,

	.signal_map                    = signal_map_openbsd,
	.if_flag_map                   = if_flag_map_openbsd,

	.show_clocks                   = show_clocks_generic,
	.show_confstrs                 = show_confstrs_generic,
	.show_cpu_affinities           = show_cpu_affinities_generic,
	.show_fds                      = show_fds_generic,
	.show_mounts                   = show_mounts_openbsd,
	.show_rlimits                  = show_rlimits_generic,
	.show_timezone                 = show_timezone_generic,
	.show_libs                     = show_libs_generic,

	.handle_proc_branch            = handle_proc_branch_openbsd,
};
