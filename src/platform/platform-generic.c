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

#include "procenv.h"

#include "platform-generic.h"
#include "messages.h"

extern struct procenv_ops *ops;

void
show_rlimits_generic (void)
{
	header ("limits");

	show_limit (RLIMIT_AS);
	show_limit (RLIMIT_CORE);
	show_limit (RLIMIT_CPU);
	show_limit (RLIMIT_DATA);
	show_limit (RLIMIT_FSIZE);
	show_limit (RLIMIT_MEMLOCK);
	show_limit (RLIMIT_NOFILE);
	show_limit (RLIMIT_NPROC);
	show_limit (RLIMIT_RSS);
	show_limit (RLIMIT_STACK);

	footer ();
}

long
get_kernel_bits_generic (void)
{
	long value;

	errno = 0;
#if defined (_SC_LONG_BIT)
	value = get_sysconf (_SC_LONG_BIT);
	if (value == -1 && errno != 0)
		return -1;
#else
	value = sizeof (long) * CHAR_BIT;
#endif
	return value;
}

void
show_fds_generic (void)
{
	int fd;
	int max;

	max = get_sysconf (_SC_OPEN_MAX);

	for (fd = 0; fd < max; fd++) {
		int    is_tty = isatty (fd);
		char  *name = NULL;
		char  *num = NULL;

		if (! fd_valid (fd))
			continue;

#if defined HAVE_TTYNAME
		name = ttyname (fd);
#endif
		appendf (&num, "%d", fd);

		object_open (false);

		section_open (num);

		entry ("terminal", "%s", is_tty ? YES_STR : NO_STR);
		entry ("valid", "%s", fd_valid (fd) ? YES_STR : NO_STR);
		entry ("device", "%s", name ? name : NA_STR);

		section_open ("capabilities");

		if (ops->show_fd_capabilities)
			ops->show_fd_capabilities (fd);

		section_close ();

		section_close ();

		object_close (false);

		free (num);
	}
}

void
show_confstrs_generic (void)
{
#if defined (_CS_GNU_LIBC_VERSION)
	show_confstr (_CS_GNU_LIBC_VERSION);
#endif

#if defined (_CS_GNU_LIBPTHREAD_VERSION)
	show_confstr (_CS_GNU_LIBPTHREAD_VERSION);
#endif

#if defined (_CS_LFS64_CFLAGS)
	show_confstr (_CS_LFS64_CFLAGS);
#endif

#if defined (_CS_LFS64_LDFLAGS)
	show_confstr (_CS_LFS64_LDFLAGS);
#endif

#if defined (_CS_LFS64_LIBS)
	show_confstr (_CS_LFS64_LIBS);
#endif

#if defined (_CS_LFS64_LINTFLAGS)
	show_confstr (_CS_LFS64_LINTFLAGS);
#endif

#if defined (_CS_LFS_CFLAGS)
	show_confstr (_CS_LFS_CFLAGS);
#endif

#if defined (_CS_LFS_LDFLAGS)
	show_confstr (_CS_LFS_LDFLAGS);
#endif

#if defined (_CS_LFS_LIBS)
	show_confstr (_CS_LFS_LIBS);
#endif

#if defined (_CS_LFS_LINTFLAGS)
	show_confstr (_CS_LFS_LINTFLAGS);
#endif

#if defined (_CS_PATH)
	show_confstr (_CS_PATH);
#endif

#if defined (_CS_POSIX_V6_ILP32_OFF32)
	show_confstr (_CS_POSIX_V6_ILP32_OFF32);
#endif

#if defined (_CS_POSIX_V6_ILP32_OFF32_CFLAGS)
	show_confstr (_CS_POSIX_V6_ILP32_OFF32_CFLAGS);
#endif

#if defined (_CS_POSIX_V6_ILP32_OFF32_LDFLAGS)
	show_confstr (_CS_POSIX_V6_ILP32_OFF32_LDFLAGS);
#endif

#if defined (_CS_POSIX_V6_ILP32_OFF32_LIBS)
	show_confstr (_CS_POSIX_V6_ILP32_OFF32_LIBS);
#endif

#if defined (_CS_POSIX_V6_ILP32_OFF32_LINTFLAGS)
	show_confstr (_CS_POSIX_V6_ILP32_OFF32_LINTFLAGS);
#endif

#if defined (_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS)
	show_confstr (_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS);
#endif

#if defined (_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS)
	show_confstr (_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS);
#endif

#if defined (_CS_POSIX_V6_ILP32_OFFBIG_LIBS)
	show_confstr (_CS_POSIX_V6_ILP32_OFFBIG_LIBS);
#endif

#if defined (_CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS)
	show_confstr (_CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS);
#endif

#if defined (_CS_POSIX_V6_LP64_OFF64_CFLAGS)
	show_confstr (_CS_POSIX_V6_LP64_OFF64_CFLAGS);
#endif

#if defined (_CS_POSIX_V6_LP64_OFF64_LDFLAGS)
	show_confstr (_CS_POSIX_V6_LP64_OFF64_LDFLAGS);
#endif

#if defined (_CS_POSIX_V6_LP64_OFF64_LIBS)
	show_confstr (_CS_POSIX_V6_LP64_OFF64_LIBS);
#endif

#if defined (_CS_POSIX_V6_LP64_OFF64_LINTFLAGS)
	show_confstr (_CS_POSIX_V6_LP64_OFF64_LINTFLAGS);
#endif

#if defined (_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS)
	show_confstr (_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS);
#endif

#if defined (_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS)
	show_confstr (_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS);
#endif

#if defined (_CS_POSIX_V6_LPBIG_OFFBIG_LIBS)
	show_confstr (_CS_POSIX_V6_LPBIG_OFFBIG_LIBS);
#endif

#if defined (_CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS)
	show_confstr (_CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS);
#endif

#if defined (_CS_POSIX_V7_ILP32_OFF32_CFLAGS)
	show_confstr (_CS_POSIX_V7_ILP32_OFF32_CFLAGS);
#endif

#if defined (_CS_POSIX_V7_ILP32_OFF32_LDFLAGS)
	show_confstr (_CS_POSIX_V7_ILP32_OFF32_LDFLAGS);
#endif

#if defined (_CS_POSIX_V7_ILP32_OFF32_LIBS)
	show_confstr (_CS_POSIX_V7_ILP32_OFF32_LIBS);
#endif

#if defined (_CS_POSIX_V7_ILP32_OFF32_LINTFLAGS)
	show_confstr (_CS_POSIX_V7_ILP32_OFF32_LINTFLAGS);
#endif

#if defined (_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS)
	show_confstr (_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS);
#endif

#if defined (_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS)
	show_confstr (_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS);
#endif

#if defined (_CS_POSIX_V7_ILP32_OFFBIG_LIBS)
	show_confstr (_CS_POSIX_V7_ILP32_OFFBIG_LIBS);
#endif

#if defined (_CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGS)
	show_confstr (_CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGS);
#endif

#if defined (_CS_POSIX_V7_LP64_OFF64_CFLAGS)
	show_confstr (_CS_POSIX_V7_LP64_OFF64_CFLAGS);
#endif

#if defined (_CS_POSIX_V7_LP64_OFF64_LDFLAGS)
	show_confstr (_CS_POSIX_V7_LP64_OFF64_LDFLAGS);
#endif

#if defined (_CS_POSIX_V7_LP64_OFF64_LIBS)
	show_confstr (_CS_POSIX_V7_LP64_OFF64_LIBS);
#endif

#if defined (_CS_POSIX_V7_LP64_OFF64_LINTFLAGS)
	show_confstr (_CS_POSIX_V7_LP64_OFF64_LINTFLAGS);
#endif

#if defined (_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS)
	show_confstr (_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS);
#endif

#if defined (_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS)
	show_confstr (_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS);
#endif

#if defined (_CS_POSIX_V7_LPBIG_OFFBIG_LIBS)
	show_confstr (_CS_POSIX_V7_LPBIG_OFFBIG_LIBS);
#endif

#if defined (_CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGS)
	show_confstr (_CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGS);
#endif

#if defined (_CS_V5_WIDTH_RESTRICTED_ENVS)
	show_confstr (_CS_V5_WIDTH_RESTRICTED_ENVS);
#endif

#if defined (_CS_V5_WIDTH_RESTRICTED_ENVS)
	_show_confstr (_CS_V5_WIDTH_RESTRICTED_ENVS, "_XBS5_WIDTH_RESTRICTED_ENVS");
#endif

#if defined (_CS_V5_WIDTH_RESTRICTED_ENVS)
	_show_confstr (_CS_V5_WIDTH_RESTRICTED_ENVS, "XBS5_WIDTH_RESTRICTED_ENVS");
#endif

#if defined (_CS_V6_WIDTH_RESTRICTED_ENVS)
	show_confstr (_CS_V6_WIDTH_RESTRICTED_ENVS);
#endif

#if defined (_CS_V6_WIDTH_RESTRICTED_ENVS)
	_show_confstr (_CS_V6_WIDTH_RESTRICTED_ENVS, "POSIX_V6_WIDTH_RESTRICTED_ENVS,_POSIX_V6_WIDTH_RESTRICTED_ENVS");
#endif

#if defined (_CS_V7_WIDTH_RESTRICTED_ENVS)
	show_confstr (_CS_V7_WIDTH_RESTRICTED_ENVS);
#endif

#if defined (_CS_XBS5_ILP32_OFF32_CFLAGS)
	show_confstr (_CS_XBS5_ILP32_OFF32_CFLAGS);
#endif

#if defined (_CS_XBS5_ILP32_OFF32_LDFLAGS)
	show_confstr (_CS_XBS5_ILP32_OFF32_LDFLAGS);
#endif

#if defined (_CS_XBS5_ILP32_OFF32_LIBS)
	show_confstr (_CS_XBS5_ILP32_OFF32_LIBS);
#endif

#if defined (_CS_XBS5_ILP32_OFF32_LINTFLAGS)
	show_confstr (_CS_XBS5_ILP32_OFF32_LINTFLAGS);
#endif

#if defined (_CS_XBS5_ILP32_OFFBIG_CFLAGS)
	show_confstr (_CS_XBS5_ILP32_OFFBIG_CFLAGS);
#endif

#if defined (_CS_XBS5_ILP32_OFFBIG_LDFLAGS)
	show_confstr (_CS_XBS5_ILP32_OFFBIG_LDFLAGS);
#endif

#if defined (_CS_XBS5_ILP32_OFFBIG_LIBS)
	show_confstr (_CS_XBS5_ILP32_OFFBIG_LIBS);
#endif

#if defined (_CS_XBS5_ILP32_OFFBIG_LINTFLAGS)
	show_confstr (_CS_XBS5_ILP32_OFFBIG_LINTFLAGS);
#endif

#if defined (_CS_XBS5_LP64_OFF64_CFLAGS)
	show_confstr (_CS_XBS5_LP64_OFF64_CFLAGS);
#endif

#if defined (_CS_XBS5_LP64_OFF64_LDFLAGS)
	show_confstr (_CS_XBS5_LP64_OFF64_LDFLAGS);
#endif

#if defined (_CS_XBS5_LP64_OFF64_LIBS)
	show_confstr (_CS_XBS5_LP64_OFF64_LIBS);
#endif

#if defined (_CS_XBS5_LP64_OFF64_LINTFLAGS)
	show_confstr (_CS_XBS5_LP64_OFF64_LINTFLAGS);
#endif

#if defined (_CS_XBS5_LPBIG_OFFBIG_CFLAGS)
	show_confstr (_CS_XBS5_LPBIG_OFFBIG_CFLAGS);
#endif

#if defined (_CS_XBS5_LPBIG_OFFBIG_LDFLAGS)
	show_confstr (_CS_XBS5_LPBIG_OFFBIG_LDFLAGS);
#endif

#if defined (_CS_XBS5_LPBIG_OFFBIG_LIBS)
	show_confstr (_CS_XBS5_LPBIG_OFFBIG_LIBS);
#endif

#if defined (_CS_XBS5_LPBIG_OFFBIG_LINTFLAGS)
	show_confstr (_CS_XBS5_LPBIG_OFFBIG_LINTFLAGS);
#endif
}

#if defined (PROCENV_PLATFORM_LINUX) || \
    defined (PROCENV_PLATFORM_BSD)   || \
    defined (PROCENV_PLATFORM_HURD)

/* Display cpu affinities in the same compressed but reasonably
 * human-readable fashion as /proc/self/status:Cpus_allowed_list under Linux.
 */ 
void
show_cpu_affinities_generic (void)
{
	PROCENV_CPU_SET_TYPE *cpu_set;
	char                 *cpu_list = NULL;
	PROCENV_CPU_TYPE      cpu;
	PROCENV_CPU_TYPE      max;

	/* true if any enabled cpus have been displayed yet */
	int                   displayed = false;

	/* Number of cpu's in *current* range */
	size_t                count = 0;

	/* Only valid to read these when count is >0 */
	size_t                last = 0;
	size_t                first = 0;

	if (! ops->get_cpuset)
		return;

	if (! ops->cpuset_has_cpu)
		return;

	cpu_set = ops->get_cpuset();
	if (! cpu_set)
		return;

	// FIXME: should be handled by get_cpuset()!!
#if 1
	max = get_sysconf (_SC_NPROCESSORS_ONLN);
#endif

	for (cpu = 0; cpu < max; cpu++) {

		if (ops->cpuset_has_cpu (cpu_set, cpu)) {
			/* Record first entry in the range */
			if (! count)
				first = cpu;

			last = cpu;
			count++;
		} else {
			if (count) {
				if (first == last) {
					appendf (&cpu_list, "%s%d",
							displayed ? "," : "",
							first);
				} else {
					appendf (&cpu_list, "%s%d-%d",
							displayed ? "," : "",
							first, last);
				}
				displayed = true;
			}

			/* Reset */
			count = 0;
		}
	}

	if (count) {
		if (first == last) {
			appendf (&cpu_list, "%s%d",
					displayed ? "," : "",
					first);
		} else {
			appendf (&cpu_list, "%s%d-%d",
					displayed ? "," : "",
					first, last);
		}
	}

	entry ("affinity list", "%s", cpu_list ? cpu_list : "-1");

	if (ops->free_cpuset)
		ops->free_cpuset (cpu_set);

	free (cpu_list);
}

void
show_pathconfs (ShowMountType what,
		const char *dir)
{
	assert (dir);

	if (what == SHOW_PATHCONF) {
		header (dir);
	} else {
		header ("pathconf");
	}

#if defined (_PC_ALLOC_SIZE_MIN)
	show_pathconf (what, dir, _PC_ALLOC_SIZE_MIN);
#endif

#if defined (_PC_ASYNC_IO)
	show_pathconf (what, dir, _PC_ASYNC_IO);
#endif

#if defined (_PC_CHOWN_RESTRICTED)
	show_pathconf (what, dir, _PC_CHOWN_RESTRICTED);
#endif

#if defined (_PC_FILESIZEBITS)
	show_pathconf (what, dir, _PC_FILESIZEBITS);
#endif

#if defined (_PC_LINK_MAX)
	show_pathconf (what, dir, _PC_LINK_MAX);
#endif

#if defined (_PC_MAX_CANON)
	show_pathconf (what, dir, _PC_MAX_CANON);
#endif

#if defined (_PC_MAX_INPUT)
	show_pathconf (what, dir, _PC_MAX_INPUT);
#endif

#if defined (_PC_NAME_MAX)
	show_pathconf (what, dir, _PC_NAME_MAX);
#endif

#if defined (_PC_NO_TRUNC)
	show_pathconf (what, dir, _PC_NO_TRUNC);
#endif

#if defined (_PC_PATH_MAX)
	show_pathconf (what, dir, _PC_PATH_MAX);
#endif

#if defined (_PC_PIPE_BUF)
	show_pathconf (what, dir, _PC_PIPE_BUF);
#endif

#if defined (_PC_PRIO_IO)
	show_pathconf (what, dir, _PC_PRIO_IO);
#endif

#if defined (_PC_REC_INCR_XFER_SIZE)
	show_pathconf (what, dir, _PC_REC_INCR_XFER_SIZE);
#endif

#if defined (_PC_REC_MAX_XFER_SIZE)
	show_pathconf (what, dir, _PC_REC_MAX_XFER_SIZE);
#endif

#if defined (_PC_REC_MIN_XFER_SIZE)
	show_pathconf (what, dir, _PC_REC_MIN_XFER_SIZE);
#endif

#if defined (_PC_REC_XFER_ALIGN)
	show_pathconf (what, dir, _PC_REC_XFER_ALIGN);
#endif

#if defined (_PC_SOCK_MAXBUF)
	show_pathconf (what, dir, _PC_SOCK_MAXBUF);
#endif

#if defined (_PC_SYMLINK_MAX)
	show_pathconf (what, dir, _PC_SYMLINK_MAX);
#endif

#if defined (_PC_2_SYMLINKS)
	show_pathconf (what, dir, _PC_2_SYMLINKS);
#endif

#if defined (_PC_SYNC_IO)
	show_pathconf (what, dir, _PC_SYNC_IO);
#endif

#if defined (_PC_VDISABLE)
	show_pathconf (what, dir, _PC_VDISABLE);
#endif

	footer ();
}

#endif /* PROCENV_PLATFORM_LINUX || PROCENV_PLATFORM_FREEBSD || PROCENV_PLATFORM_HURD */

#if defined (PROCENV_PLATFORM_BSD)

char *
get_mount_opts_generic_bsd (const struct procenv_map64 *opts, uint64_t flags)
{
	const struct procenv_map64 *opt;
	char                       *str = NULL;
	size_t                      len = 0;
	size_t                      total = 0;
	int                         count = 0;

	if (! flags)
		return strdup ("");

	/* Calculate how much space we need to allocate by iterating
	 * array for the first time.
	 */
	for (opt = opts; opt && opt->name; opt++) {
		if (flags & opt->num) {
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
	for (opt = opts; opt && opt->name; opt++) {
		if (flags & opt->num) {
			strcat (str, opt->name);
			if (count > 1)
				strcat (str, ",");
			count--;
		}
	}

	return str;
}

void
show_mounts_generic_bsd (ShowMountType what, const struct procenv_map64 *mntopt_map)
{
	procenv_mnt_type         *mounts;
	procenv_mnt_type         *mnt;
	int                       count;
	unsigned int              major = 0;
	unsigned int              minor = 0;
	int                       i;
	unsigned                  multiplier = 0;
	PROCENV_STATFS_INT_TYPE   blocks;
	PROCENV_STATFS_INT_TYPE   bfree;
	PROCENV_STATFS_INT_TYPE   bavail;
	PROCENV_STATFS_INT_TYPE   used;

	common_assert ();
	assert (mntopt_map);

	/* Note that returned memory cannot be freed (by us) */
	count = getmntinfo (&mounts, MNT_WAIT);

	if (! count)
		die ("unable to query mount info");

	mnt = mounts;

	for (i = 0; i < count; i++) {
		char *opts = NULL;

		opts = get_mount_opts_generic_bsd (mntopt_map, PROCENV_MNT_GET_FLAGS (mnt));
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
					2 * sizeof (PROCENV_MNT_GET_FSID (mnt)[0]),
					PROCENV_MNT_GET_FSID (mnt)[0],
					2 * sizeof (PROCENV_MNT_GET_FSID (mnt)[1]),
					PROCENV_MNT_GET_FSID (mnt)[1]);

			entry ("optimal block size", "%" PROCENV_STATFS_INT_FMT,
					mnt->f_bsize);

			section_open ("blocks");

			entry ("size", "%lu bytes", DF_BLOCK_SIZE);

			entry ("total", "%" PROCENV_STATFS_INT_FMT, blocks);
			entry ("used", "%"PROCENV_STATFS_INT_FMT,  used);
			entry ("free", "%" PROCENV_STATFS_INT_FMT, bfree);
			entry ("available", "%" PROCENV_STATFS_INT_FMT, bavail);

			section_close ();

			section_open ("files/inodes");

			entry ("total", "%" PROCENV_STATFS_INT_FMT, mnt->f_files);
			entry ("used", "%" PROCENV_STATFS_INT_FMT,
					mnt->f_files - mnt->f_ffree);
			entry ("free", "%" PROCENV_STATFS_INT_FMT, mnt->f_ffree);

			section_close ();

			section_close ();
		}

		if (what == SHOW_ALL || what == SHOW_PATHCONF)
			show_pathconfs (what, mnt->f_mntonname);
		mnt++;

		free (opts);
	}
}
#endif /* PROCENV_PLATFORM_BSD */

#if defined (PROCENV_PLATFORM_LINUX) || defined (PROCENV_PLATFORM_HURD)

#define MOUNTS                       "/proc/mounts"

/**
 * get_canonical:
 *
 * @path: path to convert to canonical form,
 * @canonical [out]: canonical version of @path,
 * @len: Size of @canonical (should be atleast PATH_MAX).
 *
 * FIXME: this should fully resolve not just sym links but replace all
 * occurences of '../' by the appropriate direcotry!
 **/
int
get_canonical_generic_linux (const char *path, char *canonical, size_t len)
{
	ssize_t  bytes;
	int      ret = true;

	assert (path);
	assert (canonical);
	assert (len);

	bytes = readlink (path, canonical, len);
	if (bytes < 0) {
		sprintf (canonical, UNKNOWN_STR);
		ret = false;
	} else {
		canonical[bytes <= len ? bytes : len] = '\0';
	}

	return ret;
}

void
show_mounts_generic_linux (ShowMountType what)
{
	FILE            *mtab;
	struct mntent   *mnt;
	struct statvfs   fs;
	unsigned int     major = 0;
	unsigned int     minor = 0;
	int              have_stats;
	char             canonical[PATH_MAX];

	mtab = fopen (MOUNTS, "r");

	if (! mtab)
		return;

	while ((mnt = getmntent (mtab))) {
		have_stats = true;

		if (what == SHOW_ALL || what == SHOW_MOUNTS) {
			unsigned multiplier = 0;
			fsblkcnt_t blocks = 0;
			fsblkcnt_t bfree = 0;
			fsblkcnt_t bavail = 0;
			fsblkcnt_t used_blocks = 0;
			fsblkcnt_t used_files = 0;
			int        ret;

			if (statvfs (mnt->mnt_dir, &fs) < 0) {
				have_stats = false;
			} else {
				multiplier = fs.f_bsize / DF_BLOCK_SIZE;

				blocks = fs.f_blocks * multiplier;
				bfree = fs.f_bfree * multiplier;
				bavail = fs.f_bavail * multiplier;
				used_blocks = blocks - bfree;
				used_files = fs.f_files - fs.f_ffree;
			}

			(void)get_major_minor (mnt->mnt_dir,
					&major,
					&minor);

			assert (mnt->mnt_dir);
			section_open (mnt->mnt_dir);

			entry ("filesystem", "'%s'", mnt->mnt_fsname);

			ret = get_canonical_generic_linux (mnt->mnt_fsname, canonical, sizeof (canonical));
			entry ("canonical", "%s%s%s",
					ret ? "'" : "",
					canonical,
					ret ? "'" : "");

			entry ("type", "'%s'", mnt->mnt_type);
			entry ("options", "'%s'", mnt->mnt_opts);

			show_pathconfs (what, mnt->mnt_dir);

			section_open ("device");
			entry ("major", "%u", major);
			entry ("minor", "%u", minor);
			section_close ();

			entry ("dump frequency", "%d", mnt->mnt_freq);
			entry ("fsck pass number", "%d", mnt->mnt_passno);

			if (have_stats) {
				union fsid_u {
					unsigned long int fsid;
					unsigned int val[2];
				} fsid_val;

                memset (&fsid_val, 0, sizeof (fsid_val));

				fsid_val.fsid = fs.f_fsid;

				entry ("fsid", "%.*x%.*x", 
						2 * sizeof (fsid_val.val[0]),
						fsid_val.val[0],
						2 * sizeof (fsid_val.val[1]),
						fsid_val.val[1]);

				entry ("optimal block size", "%lu", fs.f_bsize);

				section_open ("blocks");

				entry ("size", "%lu bytes", DF_BLOCK_SIZE);
				entry ("total", "%lu", blocks);
				entry ("used", "%lu", used_blocks);
				entry ("free", "%lu", bfree);
				entry ("available", "%lu", bavail);

				section_close ();

				section_open ("files/inodes");

				entry ("total", "%lu", fs.f_files);
				entry ("used", "%lu", used_files);
				entry ("free", "%lu", fs.f_ffree);

				section_close ();
			} else {
				entry ("fsid", "%s", UNKNOWN_STR);
				entry ("optimal block size", "%s", UNKNOWN_STR);

				section_open ("blocks");

				entry ("size", "%lu bytes", DF_BLOCK_SIZE);
				entry ("total", "%s", UNKNOWN_STR);
				entry ("used", "%s", UNKNOWN_STR);
				entry ("free", "%s", UNKNOWN_STR);
				entry ("available", "%s", UNKNOWN_STR);

				section_close ();

				section_open ("files/inodes");

				entry ("total", "%s", UNKNOWN_STR);
				entry ("used", "%s", UNKNOWN_STR);
				entry ("free", "%s", UNKNOWN_STR);

				section_close ();
			}

			section_close ();
		} else {
			show_pathconfs (what, mnt->mnt_dir);
		}
	}

	fclose (mtab);
}

#endif /* PROCENV_PLATFORM_LINUX || PROCENV_PLATFORM_HURD */
