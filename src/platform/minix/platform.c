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

#include "platform-minix.h"

static struct procenv_map signal_map_minix[] = {

	mk_map_entry (SIGABRT),
	mk_map_entry (SIGALRM),
	mk_map_entry (SIGBUS),

	{ SIGCHLD, "SIGCHLD|SIGCLD" },

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
	mk_map_entry (SIGKMEM),
	mk_map_entry (SIGKMESS),
	mk_map_entry (SIGKSIG),
	mk_map_entry (SIGKSIGSM),
	mk_map_entry (SIGPWR),
	mk_map_entry (SIGSNDELAY),

	{ 0, NULL },
};

static struct procenv_map64 mntopt_map_minix[] = {

	{ MNT_ASYNC       , "async" },
	//{ MNT_AUTO        , "auto" },
	{ MNT_DISCARD     , "discard" },
	{ MNT_EXTATTR     , "extattr" },
	{ MNT_FORCE       , "force" },
	{ MNT_GETARGS     , "getargs" },
	//{ MNT_GROUPQUOTA  , "groupquota" },
	{ MNT_IGNORE      , "hidden" },
	{ MNT_LOG         , "log" },
	{ MNT_NOATIME     , "atime" },
	{ MNT_NOCOREDUMP  , "coredump" },
	{ MNT_NODEV       , "dev" },
	{ MNT_NODEVMTIME  , "devmtime" },
	{ MNT_NOEXEC      , "exec" },
	{ MNT_NOSUID      , "suid" },
	{ MNT_RDONLY      , "rdonly" },
	{ MNT_RELOAD      , "reload" },
	//{ MNT_RO          , "ro" },
	//{ MNT_RUMP        , "rump" },
	//{ MNT_RW          , "rw" },
	{ MNT_SOFTDEP     , "softdep" },
	{ MNT_SYMPERM     , "symperm" },
	//{ MNT_SYNC        , "sync" },
	{ MNT_UNION       , "union" },
	{ MNT_UPDATE      , "update" },
	//{ MNT_USERQUOTA   , "userquota" },

	{ 0, NULL }
};

static void
show_mounts_minix (ShowMountType what)
{
	show_mounts_generic_bsd (what, mntopt_map_minix);
}

static void
handle_proc_branch_minix (void)
{
	/* FIXME */
}

struct procenv_ops platform_ops =
{
    .driver                        = PROCENV_SET_DRIVER (minix),

    .get_kernel_bits               = get_kernel_bits_generic,

    .signal_map                    = signal_map_minix,

    // FIXME: add show_sysconf (which will do sysctl!)

    .handle_proc_branch            = handle_proc_branch_minix,

    .show_fds                      = show_fds_generic,

    .show_mounts                   = show_mounts_minix,
    .show_rlimits                  = show_rlimits_generic,
};
