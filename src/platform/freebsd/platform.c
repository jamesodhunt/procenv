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

#include "platform-freebsd.h"

static struct procenv_map signal_map_freebsd[] = {

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
	mk_map_entry (SIGTHR),
	mk_map_entry (SIGLIBRT),

	{ 0, NULL },
};

static struct mntopt_map {
	uint64_t   flag;
	char      *name;
} mntopt_map[] = {

	{ MNT_ACLS         , "acls" },
	{ MNT_ASYNC        , "asynchronous" },
	{ MNT_EXPORTED     , "NFS-exported" },
	{ MNT_GJOURNAL     , "gjournal" },
	{ MNT_LOCAL        , "local" },
	{ MNT_MULTILABEL   , "multilabel" },
	{ MNT_NFS4ACLS     , "nfsv4acls" },
	{ MNT_NOATIME      , "noatime" },
	{ MNT_NOCLUSTERR   , "noclusterr" },
	{ MNT_NOCLUSTERW   , "noclusterw" },
	{ MNT_NOEXEC       , "noexec" },
	{ MNT_NOSUID       , "nosuid" },
	{ MNT_NOSYMFOLLOW  , "nosymfollow" },
	{ MNT_QUOTA        , "with quotas" },
	{ MNT_RDONLY       , "read-only" },
	{ MNT_SOFTDEP      , "soft-updates" },
	{ MNT_SUIDDIR      , "suiddir" },
#if defined (MNT_SUJ)
	{ MNT_SUJ          , "journaled soft-updates" },
#endif
	{ MNT_SYNCHRONOUS  , "synchronous" },
	{ MNT_UNION        , "union" },

	{ 0, NULL }
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

	{ 0, NULL }
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

static char *
get_mount_opts_freebsd (uint64_t flags)
{
	struct mntopt_map  *opt;
	char               *str = NULL;
	size_t              len = 0;
	size_t              total = 0;
	int                 count = 0;

	if (! flags)
		return strdup ("");

	/* Calculate how much space we need to allocate by iterating
	 * array for the first time.
	 */
	for (opt = mntopt_map; opt && opt->name; opt++) {
		if (flags & opt->flag) {
			count++;
			len += strlen (opt->name);
		}
	}

	if (count > 1) {
		/* we need space for the option value themselves, plus a
		 * ", " separator between each option (except the first),
		 * and finally space for the nul terminator */
		total = len + (count-1) + 1;
	} else {
		total = len + 1;
	}

	str = calloc (total, sizeof (char));
	if (! str)
		die ("failed to allocate space for mount options");

	/* Re-iterate to construct the string. This is still going to be
	 * a lot quicker than calling malloc a stack of times.
	 */
	for (opt = mntopt_map; opt && opt->name; opt++) {
		if (flags & opt->flag) {
			strcat (str, opt->name);
			if (count > 1)
				strcat (str, ",");
			count--;
		}
	}

	return str;
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
	show_capsicum_cap (rights, CAP_LINKAT);
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
	show_capsicum_cap (rights, CAP_RENAMEAT);
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
	int               count;
	struct statfs    *mounts;
	struct statfs    *mnt;
	unsigned int      major = 0;
	unsigned int      minor = 0;
	int               i;
	unsigned          multiplier = 0;
	statfs_int_type   blocks;
	statfs_int_type   bfree;
	statfs_int_type   bavail;
	statfs_int_type   used;

	common_assert ();

	/* Note that returned memory cannot be freed (by us) */
	count = getmntinfo (&mounts, MNT_WAIT);

	if (! count)
		die ("unable to query mount info");

	mnt = mounts;

	for (i = 0; i < count; i++) {
		char *opts = NULL;

		opts = get_mount_opts_freebsd (mnt->f_flags);
		if (! opts)
			die ("cannot determine FS flags for mountpoint '%s'",
					mnt->f_mntonname);

		if (what == SHOW_ALL || what == SHOW_MOUNTS) {
			(void)get_major_minor (mnt->f_mntonname,
					&major,
					&minor);

			multiplier = mnt->f_bsize / DF_BLOCK_SIZE;
			blocks = mnt->f_blocks * multiplier;
			bfree = mnt->f_bfree * multiplier;
			bavail = mnt->f_bavail * multiplier;
			used = blocks - bfree;

			assert (mnt->f_mntfromname);
			section_open (mnt->f_mntfromname);

			entry ("dir", "'%s'", mnt->f_mntonname);
			entry ("type", "%s", mnt->f_fstypename);
			entry ("options", "'%s'", opts);

			section_open ("device");
			entry ("major", "%u", major);
			entry ("minor", "%u", minor);
			section_close ();

			entry ("fsid", "%.*x%.*x", 
					/* Always zero on BSD? */
					2 * sizeof (mnt->f_fsid.val[0]),
					mnt->f_fsid.val[0],
					2 * sizeof (mnt->f_fsid.val[1]),
					mnt->f_fsid.val[1]);

			entry ("optimal block size", "%" statfs_int_fmt,
					mnt->f_bsize);

			section_open ("blocks");

			entry ("size", "%lu bytes", DF_BLOCK_SIZE);

			entry ("total", "%" statfs_int_fmt, blocks);
			entry ("used", "%"statfs_int_fmt,  used);
			entry ("free", "%" statfs_int_fmt, bfree);
			entry ("available", "%" statfs_int_fmt, bavail);

			section_close ();

			section_open ("files/inodes");

			entry ("total", "%" statfs_int_fmt, mnt->f_files);
			entry ("used", "%" statfs_int_fmt,
					mnt->f_files - mnt->f_ffree);
			entry ("free", "%" statfs_int_fmt, mnt->f_ffree);

			section_close ();

			section_close ();
		}

		if (what == SHOW_ALL || what == SHOW_PATHCONF)
			show_pathconfs (what, mnt->f_mntonname);
		mnt++;

		free (opts);
	}
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

/* Who would have thought handling PIDs was so tricky? */
static void
handle_proc_branch_freebsd (void)
{
	int                  count = 0;
	int                  i;
	char                 errors[_POSIX2_LINE_MAX];
	kvm_t               *kvm;
	struct kinfo_proc   *procs;
	struct kinfo_proc   *p;
	pid_t                self, current;
	int                  done = false;
	char                *str = NULL;
	pid_t                ultimate_parent = 0;

	common_assert ();

	self = current = getpid ();

	kvm = kvm_openfiles (NULL, _PATH_DEVNULL, NULL, O_RDONLY, errors);
	if (! kvm)
		die ("unable to open kvm");

	procs = kvm_getprocs (kvm, KERN_PROC_PROC, 0, &count);
	if (! procs)
		die ("failed to get process info");

	/* Calculate the lowest PID number which gives us the ultimate
	 * parent of all processes.
	 *
	 * On BSD sytems, normally PID 0 ('[kernel]') is the ultimate
	 * parent rather than PID 1 ('init').
	 *
	 * However, this doesn't work in a BSD jail since in that
	 * environment:
	 *
	 * - there is no init process visible.
	 * - there is no kernel thread visible.
	 * - the ultimate parent PID will either by 1 (the "invisible"
	 *   init process) or 'n' where 'n' is a PID>1 which is also
	 *   "invisible" (since it lives outside the jail in the host
	 *   environment).
	 *
	 * Confused yet?
	 */

	p = &procs[0];
	ultimate_parent = p->ki_pid;

	for (i = 1; i < count; i++) {
		p = &procs[i];
		if (p->ki_pid < ultimate_parent)
			ultimate_parent = p->ki_pid;
	}

	while (! done) {
		for (i = 0; i < count && !done; i++) {
			p = &procs[i];

			if (p->ki_pid == current) {
				if (misc.in_jail) {
					struct kinfo_proc   *p2;
					int                  ppid_found = false;
					int                  j;

					/* Determine if the parent PID
					 * actually exists within the
					 * jail.
					 */
					for (j = 0; j < count; j++) {
						p2 =  &procs[j];

						if (p2->ki_pid == p->ki_ppid) {
							ppid_found = true;
							break;
						}
					}

					if (p->ki_ppid == 1 || (p->ki_ppid && ! ppid_found)) {
						/* Found the "last" PID (whose parent is either
						 * the "invisible init" or which exists outside the jail)
						 * so record it and hop out.
						 */
						appendf (&str, "%d ('%s') %d (%s)",
								(int)current, p->ki_comm,
								p->ki_ppid, UNKNOWN_STR);
						done = true;
						break;
					} else {
						/* Found a valid parent pid */
						appendf (&str, "%d ('%s'), ",
								(int)current, p->ki_comm);
					}

				} else if (! ultimate_parent && current == ultimate_parent) {

					/* Found the "last" PID so record it and hop out */
					appendf (&str, "%d ('%s')",
							(int)current, p->ki_comm);
					done = true;
					break;
				} else {
					/* Found a valid parent pid */
					appendf (&str, "%d ('%s'), ",
							(int)current, p->ki_comm);
				}

				/* Move on */
				current = p->ki_ppid;
			}
		}
	}

	if (kvm_close (kvm) < 0)
		die ("failed to close kvm");

	entry ("ancestry", "%s", str);
	free (str);
}

struct procenv_ops platform_ops =
{
    .driver                        = PROCENV_SET_DRIVER (freebsd),

    .get_user_misc                 = get_user_misc_freebsd,
    .get_kernel_bits               = get_kernel_bits_generic,

    .signal_map                    = signal_map_freebsd,
    .if_flag_map                   = if_flag_map_freebsd,

    .show_confstrs                 = show_confstrs_generic,
    .show_cpu                      = show_cpu_freebsd,
    .show_cpu_affinities           = show_cpu_affinities_generic,
    .show_fd_capabilities          = show_fd_capabilities_freebsd,
    .show_fds                      = show_fds_generic,
    .show_mounts                   = show_mounts_freebsd,
    .show_rlimits                  = show_rlimits_generic,

    .handle_proc_branch            = handle_proc_branch_freebsd,
};
