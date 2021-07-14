/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#include "platform-hurd.h"

static struct procenv_map signal_map_hurd[] = {

	mk_map_entry (SIGABRT),
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
	mk_map_entry (SIGTTOU),
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
	mk_map_entry (SIGLOST),

	{ NULL, 0 },
};

static struct procenv_map if_flag_map_hurd[] = {
	mk_map_entry (IFF_UP),
	mk_map_entry (IFF_BROADCAST),
	mk_map_entry (IFF_DEBUG),
	mk_map_entry (IFF_LOOPBACK),
	mk_map_entry (IFF_POINTOPOINT),
	mk_map_entry (IFF_RUNNING),
	mk_map_entry (IFF_NOARP),
	mk_map_entry (IFF_PROMISC),
	mk_map_entry (IFF_ALLMULTI),
	mk_map_entry (IFF_MULTICAST),

	{ NULL, 0 },
};

struct procenv_ops platform_ops =
{
	.driver                        = PROCENV_SET_DRIVER (hurd),

	.get_kernel_bits               = get_kernel_bits_generic,
	.get_mtu                       = get_mtu_generic,
	.get_time                      = get_time_generic,

	.signal_map                    = signal_map_hurd,
	.if_flag_map                   = if_flag_map_hurd,

	.show_clocks                   = show_clocks_generic,
	.show_confstrs                 = show_confstrs_generic,
	.show_cpu_affinities           = show_cpu_affinities_generic,
	.show_fds                      = show_fds_generic,
	.show_mounts                   = show_mounts_generic_linux,
	.show_rlimits                  = show_rlimits_generic,
	.show_libs                     = show_libs_generic,
};
