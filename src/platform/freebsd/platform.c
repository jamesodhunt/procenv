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

#include "platform-freebsd.h"

static struct procenv_map signal_map_freebsd[] = {

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
	mk_map_entry (SIGTHR),
	mk_map_entry (SIGLIBRT),

	{ NULL, 0 },
};

static struct procenv_map64 mntopt_map_freebsd[] = {

	{ "acls" , MNT_ACLS },
	{ "asynchronous" , MNT_ASYNC },
	{ "NFS-exported" , MNT_EXPORTED },
	{ "gjournal" , MNT_GJOURNAL },
	{ "local" , MNT_LOCAL },
	{ "multilabel" , MNT_MULTILABEL },
	{ "nfsv4acls" , MNT_NFS4ACLS },
	{ "noatime" , MNT_NOATIME },
	{ "noclusterr" , MNT_NOCLUSTERR },
	{ "noclusterw" , MNT_NOCLUSTERW },
	{ "noexec" , MNT_NOEXEC },
	{ "nosuid" , MNT_NOSUID },
	{ "nosymfollow" , MNT_NOSYMFOLLOW },
	{ "with-quotas" , MNT_QUOTA },
	{ "read-only" , MNT_RDONLY },
	{ "soft-updates" , MNT_SOFTDEP },
	{ "suiddir" , MNT_SUIDDIR },
#if defined (MNT_SUJ)
	{ "journaled-soft-updates" , MNT_SUJ },
#endif
	{ "synchronous" , MNT_SYNCHRONOUS },
	{ "union" , MNT_UNION },

	{ NULL, 0 },
};

static struct procenv_map if_flag_map_freebsd[] = {
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

	{ NULL, 0 },
};

static void
get_user_misc_freebsd (struct procenv_user *user,
		struct procenv_misc *misc)
{
	char                 errors[_POSIX2_LINE_MAX];
	kvm_t               *kvm;
	struct kinfo_proc   *proc;
	int                  ignored;

	assert (user);
	assert (misc);

	kvm = kvm_openfiles (NULL, _PATH_DEVNULL, NULL, O_RDONLY, errors);
	if (! kvm)
		die ("unable to open kvm");

	proc = kvm_getprocs (kvm, KERN_PROC_PID, user->pid, &ignored);
	if (! proc)
		die ("failed to get process info");

	misc->in_jail = (proc->ki_flag & P_JAILED) ? true : false;
	strcpy (user->proc_name, proc->ki_comm);

	if (kvm_close (kvm) < 0)
		die ("failed to close kvm");
}

static void
show_fd_capabilities_freebsd (int fd)
{
	int           ret;
	u_int         mode;
	cap_rights_t  rights;

	ret = cap_getmode (&mode);
	if (ret < 0) {
		/* No Capsicum support */
		goto out;
	}

	ret = cap_rights_get (fd, &rights);
	if (ret < 0) {
		/* Cannot query capabilities */
		goto out;
	}

	show_capsicum_cap (rights, CAP_ACCEPT);
	show_capsicum_cap (rights, CAP_ACL_CHECK);
	show_capsicum_cap (rights, CAP_ACL_DELETE);
	show_capsicum_cap (rights, CAP_ACL_GET);
	show_capsicum_cap (rights, CAP_ACL_SET);
	show_capsicum_cap (rights, CAP_BIND);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_BINDAT);
	show_capsicum_cap (rights, CAP_CHFLAGSAT);
#endif
	show_capsicum_cap (rights, CAP_CONNECT);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_CONNECTAT);
#endif
	show_capsicum_cap (rights, CAP_CREATE);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_EVENT);
#endif
	show_capsicum_cap (rights, CAP_EXTATTR_DELETE);
	show_capsicum_cap (rights, CAP_EXTATTR_GET);
	show_capsicum_cap (rights, CAP_EXTATTR_LIST);
	show_capsicum_cap (rights, CAP_EXTATTR_SET);
	show_capsicum_cap (rights, CAP_FCHDIR);
	show_capsicum_cap (rights, CAP_FCHFLAGS);
	show_capsicum_cap (rights, CAP_FCHMOD);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_FCHMODAT);
#endif
	show_capsicum_cap (rights, CAP_FCHOWN);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_FCHOWNAT);
#endif
	show_capsicum_cap (rights, CAP_FCNTL);
	show_capsicum_cap (rights, CAP_FEXECVE);
	show_capsicum_cap (rights, CAP_FLOCK);
	show_capsicum_cap (rights, CAP_FPATHCONF);
	show_capsicum_cap (rights, CAP_FSCK);
	show_capsicum_cap (rights, CAP_FSTAT);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_FSTATAT);
#endif
	show_capsicum_cap (rights, CAP_FSTATFS);
	show_capsicum_cap (rights, CAP_FSYNC);
	show_capsicum_cap (rights, CAP_FTRUNCATE);
	show_capsicum_cap (rights, CAP_FUTIMES);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_FUTIMESAT);
#endif
	show_capsicum_cap (rights, CAP_GETPEERNAME);
	show_capsicum_cap (rights, CAP_GETSOCKNAME);
	show_capsicum_cap (rights, CAP_GETSOCKOPT);
	show_capsicum_cap (rights, CAP_IOCTL);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_KQUEUE);
	show_capsicum_cap (rights, CAP_KQUEUE_CHANGE);
	show_capsicum_cap (rights, CAP_KQUEUE_EVENT);
#if defined (CAP_LINKAT)
	show_capsicum_cap (rights, CAP_LINKAT);
#endif
#if defined (CAP_LINKAT_SOURCE)
	show_capsicum_cap (rights, CAP_LINKAT_SOURCE);
#endif
#if defined (CAP_LINKAT_TARGET)
	show_capsicum_cap (rights, CAP_LINKAT_TARGET);
#endif

#endif
	show_capsicum_cap (rights, CAP_LISTEN);
	show_capsicum_cap (rights, CAP_LOOKUP);
	show_capsicum_cap (rights, CAP_MAC_GET);
	show_capsicum_cap (rights, CAP_MAC_SET);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_MKDIRAT);
	show_capsicum_cap (rights, CAP_MKFIFOAT);
	show_capsicum_cap (rights, CAP_MKNODAT);
	show_capsicum_cap (rights, CAP_MMAP);
	show_capsicum_cap (rights, CAP_MMAP_R);
	show_capsicum_cap (rights, CAP_MMAP_RW);
	show_capsicum_cap (rights, CAP_MMAP_RWX);
	show_capsicum_cap (rights, CAP_MMAP_RX);
	show_capsicum_cap (rights, CAP_MMAP_W);
	show_capsicum_cap (rights, CAP_MMAP_WX);
	show_capsicum_cap (rights, CAP_MMAP_X);
#endif
	show_capsicum_cap (rights, CAP_PDGETPID);
	show_capsicum_cap (rights, CAP_PDKILL);
	show_capsicum_cap (rights, CAP_PDWAIT);
	show_capsicum_cap (rights, CAP_PEELOFF);
	show_capsicum_cap (rights, CAP_POLL_EVENT);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_PREAD);
	show_capsicum_cap (rights, CAP_PWRITE);
#endif
	show_capsicum_cap (rights, CAP_READ);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_RECV);
#if defined (CAP_RENAMEAT)
	show_capsicum_cap (rights, CAP_RENAMEAT);
#endif
#if defined (CAP_RENAMEAT_SOURCE)
	show_capsicum_cap (rights, CAP_RENAMEAT_SOURCE);
#endif
#if defined (CAP_RENAMEAT_TARGET)
	show_capsicum_cap (rights, CAP_RENAMEAT_TARGET);
#endif
#endif
	show_capsicum_cap (rights, CAP_SEEK);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_SEEK_TELL);
#endif
	show_capsicum_cap (rights, CAP_SEM_GETVALUE);
	show_capsicum_cap (rights, CAP_SEM_POST);
	show_capsicum_cap (rights, CAP_SEM_WAIT);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_SEND);
#endif
	show_capsicum_cap (rights, CAP_SETSOCKOPT);
	show_capsicum_cap (rights, CAP_SHUTDOWN);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_SOCK_CLIENT);
	show_capsicum_cap (rights, CAP_SOCK_SERVER);
	show_capsicum_cap (rights, CAP_SYMLINKAT);
#endif
	show_capsicum_cap (rights, CAP_TTYHOOK);
#if __FreeBSD__ > 9
	show_capsicum_cap (rights, CAP_UNLINKAT);
#endif
	show_capsicum_cap (rights, CAP_WRITE);

out:
	/* clang requires this */
	return;
}

static void
show_mounts_freebsd (ShowMountType what)
{
	show_mounts_generic_bsd (what, mntopt_map_freebsd);
}

static void
show_cpu_freebsd (void)
{
	long                max;
	kvm_t              *kvm;
	struct kinfo_proc  *proc;
	int                 ignored;
	int                 cpu;
	char                errors[_POSIX2_LINE_MAX];

	assert (user.pid > 0);

	max = get_sysconf (_SC_NPROCESSORS_ONLN);

	kvm = kvm_openfiles (NULL, _PATH_DEVNULL, NULL, O_RDONLY, errors);
	if (! kvm)
		die ("unable to open kvm");

	proc = kvm_getprocs (kvm, KERN_PROC_PID, user.pid, &ignored);
	if (! proc)
		die ("failed to get process info");

	/* cpu values are zero-based */
	cpu = 1 + proc->ki_oncpu;

	if (kvm_close (kvm) < 0)
		die ("failed to close kvm");

	entry ("number", "%u of %lu", cpu, max);
}

static void
handle_proc_branch_freebsd (void)
{
	int                  count = 0;
	char                 errors[_POSIX2_LINE_MAX];
	kvm_t               *kvm;
	struct kinfo_proc   *procs;
	struct kinfo_proc   *p;
	char                *str = NULL;
	pid_t                ultimate_parent = 0;

	common_assert ();

	kvm = kvm_openfiles (NULL, _PATH_DEVNULL, NULL, O_RDONLY, errors);
	if (! kvm)
		die ("unable to open kvm");

	procs = kvm_getprocs (kvm, KERN_PROC_PROC, 0, &count);
	if (! procs)
		die ("failed to get process info");

	/* Walk up the process tree looking for the "ultimate parent process".
	 *
	 * Every PID has a parent PID, which has a parent PID, which has a parent
	 * PID, _et cetera_. Hence, this should be easy. The difficulty is knowing
	 * when to stop, but even then the results may appear confusing to
	 * users...
	 *
	 * On FreeBSD systems, PID 0 ('[kernel]') is the ultimate
	 * parent rather than PID 1 ('init'). And this kernel PID appears in the
	 * process table... unless you're confined in a jail. But FreeBSD also
	 * offers a few sysctl kernel options that the admin can set:
	 *
	 * - sysctl security.bsd.see_other_uids=0
	 * - sysctl security.bsd.see_other_gids=0
	 *
	 * If these security otions are set to zero, a process cannot see it's
	 * ultimate parent, unless that process is running as the super-user. But
	 * if those options are set and the process is running inside a jail, it
	 * can only see direct ancestor parent processes running with its UID/GID,
	 * unless running as root.
	 *
	 * Summary of jail PID visibility:
	 *
	 * - There is no init process visible.
	 * - There is no kernel thread visible.
	 * - The ultimate parent PID will either by 1 (the "invisible"
	 *   init process) or 'n' where 'n' is a PID>1 which is also
	 *   "invisible" (since it lives outside the jail in the host
	 *   environment).
	 *
	 * Further note that if kern.randompid=1 is set, you cannot make
	 * assumptions about PID/PPID relationships (where PID >1 atleast).
	 *
	 * Confused yet?
	 *
	 * Notes:
	 *
	 * - kvm_getprocs() doesn't return details of the kernel "pid" (0).
	 *
	 * Summary:
	 *
	 *                              +------------+
	 *                              | ultimate   |
	 * +---------+-------+----------+-----+------+-------+
	 * | secure? | jail? | user     | pid | ppid | notes |
	 * |---------+-------+----------+-----+------+-------|
	 * | n       | n     | non-root | 1   |    0 |       |
	 * | n       | n     | root     | 1   |    0 |       |
	 * |---------+-------+----------+-----+------+-------|
	 * | n       | y     | non-root | any |    1 |       |
	 * | n       | y     | root     | any |    1 |       |
	 * |---------+-------+----------+-----+------+-------|
	 * | y       | n     | non-root | any |    1 |       |
	 * | y       | n     | root     | 1   |    0 |       |
	 * |---------+-------+----------+-----+------+-------|
	 * | y       | y     | non-root | any |  any | (1)   |
	 * | y       | y     | root     | any |    1 |       |
	 * +---------+-------+----------+-----+------+-------+
	 *
	 * Key:
	 *
	 * (1) - This scenario appears awkward to determine.

	 * The solution to determine the "full" process ancestry for any PID for
	 * all cases in the table is thankfully simple:
	 *
	 * - Iterate the list of entries returned by kvm_getprocs()
	 * - Find the entry corresponding to the current process and determine the
	 *   parent PID.
	 * - Re-scan the list looking for the parent PID. If it isn't in the
	 *   table, the ancestry is complete. This works:
	 *
	 *   - In a "normal" system:
	 *
	 *     Since kvm_getprocs() doesn't return details for PID 0, it's only
	 *      "mentioned in passing" by being referred to as a PPID for PID 1.
	 *
	 *   - In a jail environment:
	 *
	 *     Since the jail hides PID 1 (and PID 0).
	 *   - In a secure system where security.bsd.see_other_uids=0 and/or
	 *     security.bsd.see_other_gids=0:
	 *
	 *     Since those options hide part of the process hierarchy.
	 */

	/* Start by finding ourself ;) */
	pid_t pid_to_find = getpid ();

	bool found_pid;

	while (true) {
		found_pid = false;

		for (int i=0; i < count; i++) {
			p = &procs[i];

			if (p->ki_pid == pid_to_find) {
				appendf (&str, "%s%d ('%s')",
						(pid_to_find == getpid () ? "" : ", "),
						(int)p->ki_pid,
						p->ki_comm);

				ultimate_parent = p->ki_ppid;
				pid_to_find = ultimate_parent;
				found_pid = true;
			}
		}

		/* If the pid wasn't found, we're either in a secure or jailed
		 * environment (where only a subset of pids is visible to us),
		 * or we tried to look for details of kernel "pseudo pid" (pid 0).
		 *
		 * Either way, we can't go any further up the process tree.
		 */
		if ((! found_pid) || pid_to_find == 0) {
			break;
		}
	}

	if (kvm_close (kvm) < 0)
		die ("failed to close kvm");

	entry ("ancestry", "%s", str);
	free (str);
}

static PROCENV_CPU_SET_TYPE *
get_cpuset_freebsd (void)
{
	static PROCENV_CPU_SET_TYPE  cpu_set_real;
	PROCENV_CPU_SET_TYPE        *cs = NULL;
	size_t                       size;

	CPU_ZERO (&cpu_set_real);
	cs = &cpu_set_real;

	size = sizeof (PROCENV_CPU_SET_TYPE);

	if (pthread_getaffinity_np (pthread_self (), size, cs))
		return NULL;

	return cs;
}

static void
free_cpuset_freebsd (PROCENV_CPU_SET_TYPE *cs)
{
	(void) cs;
}

static bool cpuset_has_cpu_freebsd (const PROCENV_CPU_SET_TYPE *cs,
		PROCENV_CPU_TYPE cpu)
{
	return CPU_ISSET (cpu, cs);
}

static void
show_clocks_freebsd (void)
{
	show_clock_res (CLOCK_REALTIME);

#if defined CLOCK_REALTIME_COARSE
	show_clock_res (CLOCK_REALTIME_COARSE);
#endif

#if defined CLOCK_REALTIME_HR
	show_clock_res (CLOCK_REALTIME_HR);
#endif

	show_clock_res (CLOCK_REALTIME_PRECISE);
	show_clock_res (CLOCK_REALTIME_FAST);
	show_clock_res (CLOCK_MONOTONIC);

#if defined CLOCK_MONOTONIC_COARSE
	show_clock_res (CLOCK_MONOTONIC_COARSE);
#endif

#if defined CLOCK_MONOTONIC_RAW
	show_clock_res (CLOCK_MONOTONIC_RAW);
#endif

	show_clock_res (CLOCK_MONOTONIC_PRECISE);
	show_clock_res (CLOCK_MONOTONIC_FAST);
	show_clock_res (CLOCK_UPTIME);
	show_clock_res (CLOCK_UPTIME_PRECISE);
	show_clock_res (CLOCK_UPTIME_FAST);
	show_clock_res (CLOCK_VIRTUAL);
	show_clock_res (CLOCK_PROF);
}

struct procenv_ops platform_ops =
{
	.driver                        = PROCENV_SET_DRIVER (freebsd),

	.get_user_misc                 = get_user_misc_freebsd,
	.get_kernel_bits               = get_kernel_bits_generic,
	.get_cpuset                    = get_cpuset_freebsd,
	.get_mtu                       = get_mtu_generic,
	.get_time                      = get_time_generic,
	.free_cpuset                   = free_cpuset_freebsd,
	.cpuset_has_cpu                = cpuset_has_cpu_freebsd,

	.signal_map                    = signal_map_freebsd,
	.if_flag_map                   = if_flag_map_freebsd,

	.show_confstrs                 = show_confstrs_generic,
	.show_cpu                      = show_cpu_freebsd,
	.show_clocks                   = show_clocks_freebsd,
	.show_cpu_affinities           = show_cpu_affinities_generic,
	.show_fd_capabilities          = show_fd_capabilities_freebsd,
	.show_fds                      = show_fds_generic,
	.show_mounts                   = show_mounts_freebsd,
	.show_rlimits                  = show_rlimits_generic,
	.show_timezone                 = show_timezone_generic,
	.show_libs                     = show_libs_generic,

	.handle_memory                 = show_memory_generic,
	.handle_proc_branch            = handle_proc_branch_freebsd,
};
