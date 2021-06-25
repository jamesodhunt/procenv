/*--------------------------------------------------------------------
 * Copyright © 2016-2021 James Hunt <jamesodhunt@gmail.com>.
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
	{ "SIGPOLL|SIGEMT" , SIGPOLL },
#elif defined (SIGEMT)
	{ "SIGPOLL|SIGEMT", SIGEMT },
#endif

	{ "SIGCHLD|SIGCLD", SIGCHLD },
	{ "SIGABRT|SIGIOT", SIGABRT },

	{ NULL , 0 },
};

static struct procenv_map64 mntopt_map_darwin[] = {

	{ "asynchronous" , MNT_ASYNC },
	{ "NFS-exported" , MNT_EXPORTED },
	{ "local"        , MNT_LOCAL },
	{ "multilabel"   , MNT_MULTILABEL },
	{ "noatime"      , MNT_NOATIME },
	{ "noexec"       , MNT_NOEXEC },
	{ "nosuid"       , MNT_NOSUID },
	{ "with-quotas"  , MNT_QUOTA },
	{ "read-only"    , MNT_RDONLY },
	{ "synchronous"  , MNT_SYNCHRONOUS },
	{ "union"        , MNT_UNION },

	{ NULL , 0}
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

	{ NULL , 0 }
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

	kern_return_t    ret;

	ret = host_get_clock_service (mach_host_self(), CALENDAR_CLOCK, &cs);
	if (ret != KERN_SUCCESS) {
		return 1;
	}

	ret = clock_get_time (cs, &mts);
	if (ret != KERN_SUCCESS) {
		return 1;
	}

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

	/* XXX: This system interface seems horribly racy - what if the number of pids
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

static void
show_memory_darwin (void)
{
	size_t bytes;
	size_t len = sizeof(bytes);
	int ret;

	size_t vm_page_size;
	size_t total_ram;
	size_t used_ram;
	size_t free_ram;

	bytes = 0;
	ret = sysctlbyname("hw.memsize", &bytes, &len, NULL, 0);
	if (ret < 0) {
		return;
	}

	total_ram = bytes;

	/*------------------------------*/

	bytes = 0;
	ret = sysctlbyname("vm.pagesize", &bytes, &len, NULL, 0);
	if (ret < 0) {
		return;
	}

	vm_page_size = bytes;

	/*------------------------------*/

	bytes = 0;
	ret = sysctlbyname("vm.page_free_count", &bytes, &len, NULL, 0);
	if (ret < 0) {
		return;
	}

	used_ram = (bytes * vm_page_size);
	free_ram = total_ram - used_ram;

	/*------------------------------*/

	mach_msg_type_number_t count;
	vm_statistics_data_t stats = { 0 };

	count = HOST_VM_INFO_COUNT;
	ret = host_statistics(mach_host_self(),
			HOST_VM_INFO,
			(host_info_t)&stats,
			&count);
	if (ret != KERN_SUCCESS) {
		return;
	}

	size_t unused_vm_bytes = stats.free_count * vm_page_size;
	size_t active_bytes = stats.active_count * vm_page_size;
	size_t inactive_bytes = stats.inactive_count * vm_page_size;
	size_t wired_bytes = stats.wire_count * vm_page_size;

	/*------------------------------*/

	section_open ("ram");

	mk_mem_section ("total", total_ram);
	mk_mem_section ("free", free_ram);
	mk_mem_section ("wired", wired_bytes);
	mk_mem_section ("unused", unused_vm_bytes);
	mk_mem_section ("active", active_bytes);
	mk_mem_section ("inactive", inactive_bytes);

	section_close ();

	/*------------------------------*/
}

static char *
get_macho_version(uint32_t version)
{
	char *str = NULL;

	if (version == (uint32_t)-1) {
		appendf(&str, "(%s)", UNKNOWN_STR);
	} else {
		/* Top 2 bytes (bits 16-31) */
		uint32_t major = version >> 16;

		/* 2nd byte (bits 9-15) */
		uint32_t minor = (version >> 8) & 0xff;

		/* 1st byte (bits 0->8) */
		uint32_t patch = version & 0xff;

		appendf(&str, "%u.%u.%u", major, minor, patch);
	}

	return str;
}

static void
handle_load_cmds(const struct mach_header *header)
{
	if (! header) return;

	uint32_t cmds = header->ncmds;

	if (cmds == 0) return;

	/* The commands immediately follow the mach header,
	 * but the size of the header varies depending on the platform!
	 */
	uintptr_t cmds_addr = (uintptr_t)header + mach_header_size(header);

	uintptr_t p = cmds_addr;

	for (uint32_t i = 0; i < cmds; i++)
	{
		struct load_command *lc = (struct load_command *)p;

		/* We only need to consider a subset of load commands
		 * to determine which shared libraries are loaded.
		 */
		bool consider = lc->cmd == LC_ID_DYLIB ||
			lc->cmd == LC_LOAD_DYLIB ||
			lc->cmd == LC_LOAD_WEAK_DYLIB ||
			lc->cmd == LC_REEXPORT_DYLIB ||
			lc->cmd == LC_LOAD_UPWARD_DYLIB ||
			lc->cmd == LC_LAZY_LOAD_DYLIB;

		if (consider) {
			struct dylib_command *dl = (struct dylib_command *)lc;

			struct dylib dylib = dl->dylib;

			// Determine shared library path
			char *path = (char *)lc + dylib.name.offset;

			char *tmp = strdup (path);
			char *name = basename (tmp);

			object_open (false);
			section_open (name);

			free (tmp);

			void *addr = dlopen(path, RTLD_LAZY);
			if (addr) {
				entry ("address", "%p", addr);
				dlclose (addr);
			}

			entry ("path", "%s", path);

			/*------------------------------*/

			section_open ("versions");

			char *current_version = get_macho_version(dylib.current_version);
			char *compat_version  = get_macho_version(dylib.compatibility_version);

			if (current_version) {
				entry ("current", "%s", current_version);
				free (current_version);
			}

			if (compat_version) {
				entry ("compatibility", "%s", compat_version);
				free (compat_version);
			}

			section_close ();

			/*------------------------------*/

			section_close ();
			object_close (false);
		}

		p += lc->cmdsize;
	}
}

static void
show_libs_darwin(void)
{
	/* First, determine the OSX name for this binary */
	int ret;
	char path[PATH_MAX];
	uint32_t size = (uint32_t)sizeof(path);

	ret = _NSGetExecutablePath(path, &size);
	if (ret != 0) return;

	int count = _dyld_image_count();

	/* Now, look for the image relating to this program */
	for (int i = 0; i < count; i++) {
		const char *name = _dyld_get_image_name(i);

		if (!name) {
			continue;
		}

		/* Only show dependencies for _ourself_ */
		if (! strcmp(name, path)) {
			const struct mach_header *header = _dyld_get_image_header(i);

			handle_load_cmds(header);
			break;
		}
	}
}

/* Darwin lacks:
 *
 * - cpusets and cpu affinities.
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
	.show_clocks                   = show_clocks_generic,
	.show_fds                      = show_fds_generic,
	.show_mounts                   = show_mounts_darwin,
	.show_rlimits                  = show_rlimits_generic,
	.show_timezone                 = show_timezone_generic,
	.show_libs                     = show_libs_darwin,

	.handle_memory                 = show_memory_darwin,
};
