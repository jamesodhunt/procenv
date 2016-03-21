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

#include "platform-linux.h"

static struct procenv_map scheduler_map[] = {

	mk_map_entry (SCHED_OTHER),
	mk_map_entry (SCHED_FIFO),
	mk_map_entry (SCHED_RR),
	mk_map_entry (SCHED_BATCH),

#ifdef SCHED_IDLE
	mk_map_entry (SCHED_IDLE),
#endif

	{ 0, NULL }
};

static struct procenv_map signal_map_linux[] = {

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
	mk_map_entry (SIGIOT),

	{SIGPOLL, "SIGPOLL|SIGIO" },

	mk_map_entry (SIGPROF),

	mk_map_entry (SIGPWR),
#ifdef SIGSTKFLT
	mk_map_entry (SIGSTKFLT),
#endif

	mk_map_entry (SIGSYS),

#ifdef SIGUNUSED
	mk_map_entry (SIGUNUSED),
#endif
	mk_map_entry (SIGURG),
	mk_map_entry (SIGVTALRM),
	mk_map_entry (SIGWINCH),
	mk_map_entry (SIGXCPU),
	mk_map_entry (SIGXFSZ),

	{ 0, NULL },
};

static struct procenv_map if_flag_map_linux[] = {
	mk_map_entry (IFF_UP),
	mk_map_entry (IFF_BROADCAST),
	mk_map_entry (IFF_DEBUG),
	mk_map_entry (IFF_LOOPBACK),
	mk_map_entry (IFF_POINTOPOINT),
	mk_map_entry (IFF_RUNNING),
	mk_map_entry (IFF_NOARP),
	mk_map_entry (IFF_PROMISC),
	mk_map_entry (IFF_NOTRAILERS),
	mk_map_entry (IFF_ALLMULTI),
	mk_map_entry (IFF_MASTER),
	mk_map_entry (IFF_SLAVE),
	mk_map_entry (IFF_MULTICAST),
	mk_map_entry (IFF_PORTSEL),
	mk_map_entry (IFF_AUTOMEDIA),
	mk_map_entry (IFF_DYNAMIC),
	mk_map_entry (IFF_LOWER_UP),
	mk_map_entry (IFF_DORMANT),
#ifdef IFF_ECHO
	mk_map_entry (IFF_ECHO),
#endif

	{ 0, NULL }
};

static struct procenv_map if_extended_flag_map_linux[] = {
#if defined (IFF_802_1Q_VLAN)
	mk_map_entry (IFF_802_1Q_VLAN),
#endif
#if defined (IFF_EBRIDGE)
	mk_map_entry (IFF_EBRIDGE),
#endif
#if defined (IFF_SLAVE_INACTIVE)
	mk_map_entry (IFF_SLAVE_INACTIVE),
#endif
#if defined (IFF_MASTER_8023AD)
	mk_map_entry (IFF_MASTER_8023AD),
#endif
#if defined (IFF_MASTER_ALB)
	mk_map_entry (IFF_MASTER_ALB),
#endif
#if defined (IFF_BONDING)
	mk_map_entry (IFF_BONDING),
#endif
#if defined (IFF_SLAVE_NEEDARP)
	mk_map_entry (IFF_SLAVE_NEEDARP),
#endif
#if defined (IFF_ISATAP)
	mk_map_entry (IFF_ISATAP),
#endif
	{ 0, NULL }
};

static struct procenv_map personality_map_linux[] = {
	mk_map_entry (PER_LINUX),
	mk_map_entry (PER_LINUX_32BIT),
	mk_map_entry (PER_SVR4),
	mk_map_entry (PER_SVR3),
	mk_map_entry (PER_SCOSVR3),
	mk_map_entry (PER_OSR5),
	mk_map_entry (PER_WYSEV386),
	mk_map_entry (PER_ISCR4),
	mk_map_entry (PER_BSD),
	mk_map_entry (PER_SUNOS),
	mk_map_entry (PER_XENIX),
#if defined (PER_LINUX32)
	mk_map_entry (PER_LINUX32),
#endif
#if defined (PER_LINUX32_3GB)
	mk_map_entry (PER_LINUX32_3GB),
#endif
	mk_map_entry (PER_IRIX32),
	mk_map_entry (PER_IRIXN32),
	mk_map_entry (PER_IRIX64),
	mk_map_entry (PER_RISCOS),
	mk_map_entry (PER_SOLARIS),
	mk_map_entry (PER_UW7),
	mk_map_entry (PER_OSF4),
	mk_map_entry (PER_HPUX),

	{ 0, NULL }
};

static struct procenv_map personality_flag_map_linux[] = {
#if defined (ADDR_COMPAT_LAYOUT)
	mk_map_entry (ADDR_COMPAT_LAYOUT),
#endif
#if defined (ADDR_LIMIT_32BIT)
	mk_map_entry (ADDR_LIMIT_32BIT),
#endif
#if defined (ADDR_LIMIT_3GB)
	mk_map_entry (ADDR_LIMIT_3GB),
#endif
#if defined (ADDR_NO_RANDOMIZE)
	mk_map_entry (ADDR_NO_RANDOMIZE),
#endif
#if defined (MMAP_PAGE_ZERO)
	mk_map_entry (MMAP_PAGE_ZERO),
#endif
#if defined (READ_IMPLIES_EXEC)
	mk_map_entry (READ_IMPLIES_EXEC),
#endif
#if defined (SHORT_INODE)
	mk_map_entry (SHORT_INODE),
#endif
#if defined (STICKY_TIMEOUTS)
	mk_map_entry (STICKY_TIMEOUTS),
#endif
#if defined (WHOLE_SECONDS)
	mk_map_entry (WHOLE_SECONDS),
#endif

	{ 0, NULL }
};

/* lifted from include/linux/ioprio.h since not available in libc */
#define IOPRIO_CLASS_SHIFT (13)
#define IOPRIO_PRIO_MASK ((1UL << IOPRIO_CLASS_SHIFT) - 1)
#define IOPRIO_PRIO_CLASS(mask) ((mask) >> IOPRIO_CLASS_SHIFT)
#define IOPRIO_PRIO_DATA(mask) ((mask) & IOPRIO_PRIO_MASK)

enum {
    IOPRIO_WHO_PROCESS = 1,
    IOPRIO_WHO_PGRP,
    IOPRIO_WHO_USER,
};

enum {
    IOPRIO_CLASS_NONE, /* FIXME: is this valid? */
	IOPRIO_CLASS_RT,
	IOPRIO_CLASS_BE,
	IOPRIO_CLASS_IDLE,
    IOPRIO_CLASS_NORMAL,
};

static struct procenv_map io_priorities_class_map[] = {
	mk_map_entry (IOPRIO_CLASS_NONE),
	mk_map_entry (IOPRIO_CLASS_RT),
	mk_map_entry (IOPRIO_CLASS_BE),
	mk_map_entry (IOPRIO_CLASS_IDLE),
	mk_map_entry (IOPRIO_CLASS_NORMAL),

	{ 0, NULL }
};

static char *
pid_to_name (pid_t pid)
{
	char   path[PATH_MAX];
	char  *name = NULL;
	FILE  *f = NULL;

	sprintf (path, "/proc/%d/cmdline", (int)pid);

	f = fopen (path, "r");
	if (! f) 
		goto out;

	/* Reuse buffer */
	if (! fgets (path, sizeof (path), f))
		goto out;

	/* Nul delimiting within /proc file will ensure we only get the
	 * program name.
	 */
	append (&name, path);

out:
	if (f)
		fclose (f);

	return name;
}

static void
get_user_misc_linux (struct procenv_user *user,
		struct procenv_misc *misc)
{
    assert (user);
    assert (misc);
    get_canonical_generic_linux (ROOT_PATH, misc->root, sizeof (misc->root));
}

static void
show_capabilities_linux (void)
{
#if defined (HAVE_SYS_CAPABILITY_H)
	int    last_known;
	cap_t  caps;

	/* Most recently-added capability that procenv knew about at
	 * compile time.
	 */
	last_known = CAP_LAST_CAP;
#endif

#if defined (HAVE_SYS_CAPABILITY_H)
	caps = cap_get_proc ();

	entry ("count (CAP_LAST_CAP+1)", "%d", CAP_LAST_CAP+1);

	if (! caps)
		goto out;

	section_open ("known");

	show_capability (caps, CAP_CHOWN);
	show_capability (caps, CAP_DAC_OVERRIDE);
	show_capability (caps, CAP_DAC_READ_SEARCH);
	show_capability (caps, CAP_FOWNER);
	show_capability (caps, CAP_FSETID);
	show_capability (caps, CAP_KILL);
	show_capability (caps, CAP_SETGID);
	show_capability (caps, CAP_SETUID);
	show_capability (caps, CAP_SETPCAP);
	show_capability (caps, CAP_LINUX_IMMUTABLE);
	show_capability (caps, CAP_NET_BIND_SERVICE);
	show_capability (caps, CAP_NET_BROADCAST);
	show_capability (caps, CAP_NET_ADMIN);
	show_capability (caps, CAP_NET_RAW);
	show_capability (caps, CAP_IPC_LOCK);
	show_capability (caps, CAP_IPC_OWNER);
	show_capability (caps, CAP_SYS_MODULE);
	show_capability (caps, CAP_SYS_RAWIO);
	show_capability (caps, CAP_SYS_CHROOT);
	show_capability (caps, CAP_SYS_PTRACE);
	show_capability (caps, CAP_SYS_PACCT);
	show_capability (caps, CAP_SYS_ADMIN);
	show_capability (caps, CAP_SYS_BOOT);
	show_capability (caps, CAP_SYS_NICE);
	show_capability (caps, CAP_SYS_RESOURCE);
	show_capability (caps, CAP_SYS_TIME);
	show_capability (caps, CAP_SYS_TTY_CONFIG);

	if (LINUX_KERNEL_MM (2, 4)) {
		show_capability (caps, CAP_MKNOD);
		show_capability (caps, CAP_LEASE);
	}

	if (LINUX_KERNEL_MMR (2, 6, 11)) {
		show_capability (caps, CAP_AUDIT_WRITE);
		show_capability (caps, CAP_AUDIT_CONTROL);
	}
	if (LINUX_KERNEL_MMR (2, 6, 24))
		show_capability (caps, CAP_SETFCAP);
	if (LINUX_KERNEL_MMR (2, 6, 25)) {
		show_capability (caps, CAP_MAC_OVERRIDE);
		show_capability (caps, CAP_MAC_ADMIN);
	}

#ifdef CAP_SYSLOG
	if (LINUX_KERNEL_MMR (2, 6, 37))
		show_capability (caps, CAP_SYSLOG);
#endif /* CAP_SYSLOG */

#ifdef CAP_WAKE_ALARM
	if (LINUX_KERNEL_MM (3, 0))
		show_capability (caps, CAP_WAKE_ALARM);
#endif /* CAP_WAKE_ALARM */

#ifdef CAP_BLOCK_SUSPEND
	if (LINUX_KERNEL_MM (3, 5))
		show_capability (caps, CAP_BLOCK_SUSPEND);
#endif /* CAP_BLOCK_SUSPEND */

#ifdef CAP_AUDIT_READ
	if (LINUX_KERNEL_MM (3, 16)) {
		show_capability (caps, CAP_AUDIT_READ);
	}
#endif /* CAP_AUDIT_READ */

	section_close ();

	/* It's possible that procenv is running on a system which has
	 * more capabilities that the system it was built on (for
	 * example, it might be running in a chroot with a newer kernel
	 * than the chroot environment). So display any unknown
	 * capabilities. We don't have their names, but it's useful to
	 * see that there are additional capabilities in available in
	 * the environment.
	 */
	section_open ("unknown");

#if defined (PR_CAPBSET_READ)
	for (int i = 1+last_known; ; i++) {
		int   ret;
		char *name = NULL;

		ret = cap_get_bound (i);
		if (ret < 0)
			break;

		/* Found an "unknown" */

		appendf (&name, "CAP_LAST_CAP+%d", i);

		_show_capability (caps, i, name);

		free (name);
	}
#endif

	cap_free (caps);

	section_close ();

#ifdef PR_GET_KEEPCAPS
	if (LINUX_KERNEL_MMR (2, 2, 18)) {
		int   ret;
		ret = prctl (PR_GET_KEEPCAPS, 0, 0, 0, 0);
		if (ret < 0)
			entry ("keep", "%s", UNKNOWN_STR);
		else
			entry ("keep", "%s", ret ? YES_STR : NO_STR);
	}
#endif


#if defined (PR_GET_SECUREBITS) && defined (HAVE_LINUX_SECUREBITS_H)
	if (LINUX_KERNEL_MMR (2, 6, 26)) {
		int ret;

		ret = prctl (PR_GET_SECUREBITS, 0, 0, 0, 0);
		if (ret < 0)
			entry ("securebits", "%s", UNKNOWN_STR);
		else {
			struct securebits_t {
				unsigned int securebits;
			} flags;
			flags.securebits = (unsigned int)ret;

			section_open ("securebits");

			entry ("value", "0x%x", flags.securebits);

			container_open ("fields");

			show_const (flags, securebits, SECBIT_KEEP_CAPS);
			show_const (flags, securebits, SECBIT_NO_SETUID_FIXUP);
			show_const (flags, securebits, SECBIT_NOROOT);

			container_close ();

			section_close ();
		}
	}
#endif

out:
    /* compiler appeasement */
    return;
#endif
}

static void
show_cgroups_linux (void)
{
	const  char  *delim = ":";
	char         *file = "/proc/self/cgroup";
	FILE         *f;
	char          buffer[1024];
	size_t        len;

	f = fopen (file, "r");

	if (! f)
		goto out;

	while (fgets (buffer, sizeof (buffer), f)) {
		char  *buf, *b;
		char  *hierarchy;
		char  *subsystems = NULL;
		char  *path;

		len = strlen (buffer);
		/* Remove NL */
		buffer[len-1] = '\0';

		buf = b = strdup (buffer);
		if (! buf)
			die ("failed to alloate storage");

		hierarchy = strsep (&b, delim);
		if (! hierarchy)
			goto next;

		/* don't fail if this returns '\0' to tolerate cgroup2 where the
		 * subsystem is always empty.
		 */
		subsystems = strsep (&b, delim);

		path = strsep (&b, delim);
		if (! path)
			goto next;

		/* FIXME: should sort by hierarchy */
		container_open (hierarchy);

		object_open (false);

		/* FIXME: should split this on comma */
		if (subsystems && *subsystems)
			entry ("subsystems", "%s", subsystems);

		entry ("path", "%s", path);

		object_close (false);

		container_close ();

next:
		free (buf);
	}

	fclose (f);

out:

    /* compiler appeasement */
    return;
}

static void
show_fds_linux (void)
{
	DIR            *dir;
	struct dirent  *ent;
	char           *prefix_path = "/proc/self/fd";
	struct stat     st;
	char            path[MAXPATHLEN];
	char            link[MAXPATHLEN];
	ssize_t         len;

	dir = opendir (prefix_path);
	if (! dir)
		return;

	while ((ent=readdir (dir)) != NULL) {
		int    fd;
		char  *num = NULL;

		if (! strcmp (ent->d_name, ".") || ! strcmp (ent->d_name, ".."))
			continue;

		sprintf (path, "%s/%s", prefix_path, ent->d_name);
		fd = atoi (ent->d_name);

		len = readlink (path, link, sizeof (link)-1);
		if (len < 0)
			/* ignore errors */
			continue;

		appendf (&num, "%d", fd);

		assert (len);
		link[len] = '\0';

		if (link[0] == '/') {

			if (stat (link, &st) < 0) {
				free (num);
				continue;
			}

			/* Ignore the last (invalid) entry */
			if (S_ISDIR (st.st_mode)) {
				free (num);
				continue;
			}
		}

		object_open (false);

		section_open (num);
		free (num);

		entry ("terminal", "%s", isatty (fd) ? YES_STR : NO_STR);
		entry ("valid", "%s", fd_valid (fd) ? YES_STR : NO_STR);
		entry ("device", "%s", link);

		section_close ();

		object_close (false);
	}

	closedir (dir);
}

static void
show_namespaces_linux (void)
{
	DIR            *dir;
	struct dirent  *ent;
	char           *prefix_path = "/proc/self/ns";
	char            path[MAXPATHLEN];
	char            link[MAXPATHLEN];
	ssize_t         len;
	PRList         *list = NULL;

	dir = opendir (prefix_path);
	if (! dir)
		goto end;

	list = pr_list_new (NULL);
	assert (list);

	while ((ent=readdir (dir)) != NULL) {
		PRList *entry;

		if (! strcmp (ent->d_name, ".") || ! strcmp (ent->d_name, ".."))
			continue;

		sprintf (path, "%s/%s", prefix_path, ent->d_name);

		len = readlink (path, link, sizeof (link)-1);
		if (len < 0)
			/* ignore errors */
			continue;

		assert (len);
		link[len] = '\0';

		entry = pr_list_new (strdup (link));
		assert (entry);

		assert (pr_list_prepend_str_sorted (list, entry));
	}

	closedir (dir);

	PR_LIST_FOREACH_SAFE (list, iter) {
		char *tmp;
		char *name;
		char *value;

		pr_list_remove (iter);

		tmp = iter->data;

		name = strsep (&tmp, ":");
		if (! name)
			goto give_up;

		value = strsep (&tmp, "]");
		if (! value)
			goto give_up;

		if (*value == '[' && *(value+1)) {
			value++;
		}

		object_open (false);
		entry (name, "%s", value);
		object_close (false);

give_up:
		free ((char *)iter->data);
		free(iter);
	}

	free_if_set (list);

end:
    /* compiler appeasement */
    return;
}

static void
show_oom_linux (void)
{
	char    *dir = "/proc/self";
	char    *files[] = { "oom_score", "oom_adj", "oom_score_adj", NULL };
	char    **file;
	FILE    *f;
	char     buffer[PROCENV_BUFFER];
	char     path[PATH_MAX];
	size_t   len;
	int      ret;
	int      seen = false;

	for (file = files; file && *file; file++) {
		ret = sprintf (path, "%s/%s", dir, *file);
		if (ret < 0)
			continue;

		f = fopen (path, "r");
		if (! f)
			continue;

		seen = true;

		while (fgets (buffer, sizeof (buffer), f)) {
			len = strlen (buffer);
			buffer[len-1] = '\0';
			entry (*file, "%s", buffer);
		}

		fclose (f);
	}

	if (! seen)
		entry ("%s", UNKNOWN_STR);
}

#if ! defined (HAVE_SCHED_GETCPU)

/* Crutch function for RHEL 5 */
static int
procenv_getcpu (void)
{
	int          cpu = -1;
	FILE        *f;
	char       **fields;
	const char  *field;
	char         buffer[1024];
	size_t       len;
	size_t       count;

	f = fopen ("/proc/self/stat", "r");
	if (! f)
		goto out;

	if (! fgets (buffer, sizeof (buffer), f))
		goto out;

	fclose (f);

	len = strlen (buffer);
	buffer[len-1] = '\0';

	count = split_fields (buffer, ' ', true, &fields);

	if (! count)
		return -1;

	if (count != 42)
		goto cleanup;

	field = fields[41];
	assert (field);
	
	cpu = atoi (field);

cleanup:

	for (len = 0; len < count; len++)
		free (fields[len]);
	free (fields);

out:
	return cpu;
}

#endif

static void
show_cpu_linux (void)
{
	int cpu = -1;
	long max;

	max = get_sysconf (_SC_NPROCESSORS_ONLN);

#if HAVE_SCHED_GETCPU
	cpu = sched_getcpu ();
	if (cpu < 0)
		goto unknown_sched_cpu;

#else
	cpu = procenv_getcpu ();
	if (cpu < 0)
		goto unknown_sched_cpu;
#endif

	/* adjust to make 1-based */
	cpu++;

	entry ("number", "%u of %ld", cpu, max);
	return;

unknown_sched_cpu:

	entry ("number", "%s of %ld", UNKNOWN_STR, max);
}

static void
show_msg_queues_linux (void)
{
	int               i;
	int               id;
	int               max;
	struct msginfo    info;
	struct msqid_ds   msqid_ds;
	struct ipc_perm  *perm;
	char              formatted_stime[CTIME_BUFFER];
	char              formatted_rtime[CTIME_BUFFER];
	char              formatted_ctime[CTIME_BUFFER];
	char             *modestr = NULL;
	char             *lspid = NULL;
	char             *lrpid = NULL;

	max = msgctl (0, MSG_INFO, (void *)&info);
	if (max < 0)
		goto out;

	section_open ("info");

	entry ("msgpool", "%d", info.msgpool);
	entry ("msgmap", "%d", info.msgmap);
	entry ("msgmax", "%d", info.msgmax);
	entry ("msgmnb", "%d", info.msgmnb);
	entry ("msgmni", "%d", info.msgmni);
	entry ("msgssz", "%d", info.msgssz);
	entry ("msgtql", "%d", info.msgtql);
	entry ("msgseg", "%d", info.msgseg);

	section_close ();

	container_open ("sets");

	object_open (false);

	for (i = 0; i <= max; i++) {
		char *id_str = NULL;

		id = msgctl (i, MSG_STAT, &msqid_ds);
		if (id < 0) {
			/* found an unused slot, so ignore it */
			continue;
		}

		perm = &msqid_ds.msg_perm;

		modestr = format_perms (perm->mode);
		if (! modestr)
			die ("failed to allocate space for permissions string");

		/* May not have been set */
		if (msqid_ds.msg_stime)
			format_time (&msqid_ds.msg_stime, formatted_stime, sizeof (formatted_stime));
		else
			sprintf (formatted_stime, "%s", NA_STR);

		/* May not have been set */
		if (msqid_ds.msg_rtime)
			format_time (&msqid_ds.msg_rtime, formatted_rtime, sizeof (formatted_rtime));
		else
			sprintf (formatted_rtime, "%s", NA_STR);

		/* May not have been set */
		if (msqid_ds.msg_ctime)
			format_time (&msqid_ds.msg_ctime, formatted_ctime, sizeof (formatted_ctime));
		else
			sprintf (formatted_ctime, "%s", NA_STR);

		lspid = pid_to_name (msqid_ds.msg_lspid);
		lrpid = pid_to_name (msqid_ds.msg_lrpid);

		appendf (&id_str, "%d", id);

		container_open (id_str);
		free (id_str);

		object_open (false);

		/* pad out to max pointer size represented in hex */
		entry ("key", "0x%.*x", POINTER_SIZE * 2, perm->__key);
		entry ("sequence", "%u", perm->__seq);

		section_open ("permissions");
		entry ("octal", "%4.4o", perm->mode);
		entry ("symbolic", "%s", modestr);
		section_close ();

		section_open ("creator");
		entry ("euid", "%u ('%s')", perm->cuid, get_user_name (perm->cuid));
		entry ("egid", "%u ('%s')", perm->cgid, get_group_name (perm->cgid));
		section_close ();

		section_open ("owner");
		entry ("uid", "%u ('%s')", perm->uid, get_user_name (perm->uid));
		entry ("gid", "%u ('%s')", perm->gid, get_group_name (perm->gid));
		section_close ();

		section_open ("times");
		entry ("last send (stime)", "%lu (%s)", msqid_ds.msg_stime, formatted_stime);
		entry ("last receive (rtime)", "%lu (%s)", msqid_ds.msg_rtime, formatted_rtime);
		entry ("last change (ctime)", "%lu (%s)", msqid_ds.msg_ctime, formatted_ctime);
		section_close ();

		entry ("queue_bytes", "%lu", msqid_ds.__msg_cbytes);

		entry ("msg_qnum", "%lu", msqid_ds.msg_qnum);
		entry ("msg_qbytes", "%lu", msqid_ds.msg_qbytes);

		entry ("last msgsnd pid", "%d (%s)", msqid_ds.msg_lspid,
				lspid ? lspid : UNKNOWN_STR);

		entry ("last msgrcv pid", "%d (%s)", msqid_ds.msg_lrpid,
				lrpid ? lrpid : UNKNOWN_STR);

		object_close (false);

		container_close ();

		free (modestr);
		if (lspid)
			free (lspid);
		if (lrpid)
			free (lrpid);
	}

	object_close (false);

	container_close ();

out:
    /* compiler appeasement */
    return;
}

static void
show_prctl_linux (void)
{
	int  rc;
	int  arg2;
	char name[17] = { 0 };

#ifdef PR_GET_ENDIAN
	if (LINUX_KERNEL_MMR (2, 6, 18)) {
		const char *value;

		rc = prctl (PR_GET_ENDIAN, &arg2, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (arg2) {
			case PR_ENDIAN_BIG:
				value = BIG_STR; 
				break;
			case PR_ENDIAN_LITTLE:
				value = LITTLE_STR; 
				break;
			case PR_ENDIAN_PPC_LITTLE:
				value = "PowerPC pseudo little endian";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("process endian", "%s", value);
	}
#endif

#ifdef PR_GET_DUMPABLE
	if (LINUX_KERNEL_MMR (2, 3, 20)) {
		const char *value;
		rc = prctl (PR_GET_DUMPABLE, 0, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (rc) {
			case 0:
				value = NO_STR;
				break;
			case 1:
				value = YES_STR;
				break;
			case 2:
				value = "root-only";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("dumpable", "%s", value);
	}
#endif

#ifdef PR_GET_FPEMU
	/* Use the earliest version where this option was introduced
	 * (for some architectures).
	 */
	if (LINUX_KERNEL_MMR (2, 4, 18)) {
		const char *value;

		rc = prctl (PR_GET_FPEMU, &arg2, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (arg2) {
			case PR_FPEMU_NOPRINT:
				value = YES_STR;
				break;
			case PR_FPEMU_SIGFPE:
				value = "send SIGFPE";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("floating point emulation", "%s", value);
	}
#endif

#ifdef PR_GET_FPEXC
	/* Use the earliest version where this option was introduced
	 * (for some architectures).
	 */
	if (LINUX_KERNEL_MMR (2, 4, 21)) {
		const char *value;

		rc = prctl (PR_GET_FPEXC, &arg2, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (arg2) {
			case PR_FP_EXC_SW_ENABLE:
				value = "software";
				break;
			case PR_FP_EXC_DISABLED:
				value = "disabled";
				break;
			case PR_FP_EXC_NONRECOV:
				value = "non-recoverable";
				break;
			case PR_FP_EXC_ASYNC:
				value = "asynchronous";
				break;
			case PR_FP_EXC_PRECISE:
				value = "precise";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("floating point exceptions", "%s", value);
	}
#endif

#ifdef PR_GET_NAME
	if (LINUX_KERNEL_MMR (2, 6, 11)) {
		rc = prctl (PR_GET_NAME, name, 0, 0, 0);
		if (rc < 0)
			entry ("process name", "%s", UNKNOWN_STR);
		else
			entry ("process name", "%s", name);
	}

#endif

#ifdef PR_GET_PDEATHSIG
	if (LINUX_KERNEL_MMR (2, 3, 15)) {
		rc = prctl (PR_GET_PDEATHSIG, &arg2, 0, 0, 0);
		if (rc < 0)
			entry ("parent death signal", "%s", UNKNOWN_STR);
		else if (rc == 0)
			entry ("parent death signal", "disabled");
		else
			entry ("parent death signal", "%d", arg2);
	}
#endif

#ifdef PR_GET_SECCOMP
	if (LINUX_KERNEL_MMR (2, 6, 23)) {
		const char *value;

		rc = prctl (PR_GET_SECCOMP, 0, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (rc) {
			case 0:
				value = "disabled";
				break;
			case 1:
				value = "read/write/exit (mode 1)";
				break;
			case 2:
				value = "BPF (mode 2)";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("secure computing", "%s", value);
	}
#endif

#ifdef PR_GET_TIMING
	/* Not 100% accurate - this option was actually
	 * introduced in 2.6.0-test4
	 */
	if (LINUX_KERNEL_MMR (2, 6, 1)) {
		const char *value;
		rc = prctl (PR_GET_TIMING, 0, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (rc) {
			case PR_TIMING_STATISTICAL:
				value = "statistical";
				break;
			case PR_TIMING_TIMESTAMP:
				value = "time-stamp";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("process timing", "%s", value);
	}
#endif

#if defined (PR_GET_TSC)
	if (LINUX_KERNEL_MMR (2, 6, 26)) {
		const char *value;

		rc = prctl (PR_GET_TSC, &arg2, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (arg2) {
			case PR_TSC_ENABLE:
				value = "enabled";
				break;
			case PR_TSC_SIGSEGV:
				value = "segmentation fault";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("timestamp counter read", "%s", value);
	}
#endif

#ifdef PR_GET_UNALIGNED
	if (LINUX_KERNEL_MMR (2, 3, 48)) {
		const char *value;

		rc = prctl (PR_GET_UNALIGNED, &arg2, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (arg2) {
			case PR_UNALIGN_NOPRINT:
				value = "fix-up";
				break;
			case PR_UNALIGN_SIGBUS:
				value = "send SIGBUS";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("unaligned access", "%s", value);
	}
#endif

#ifdef PR_MCE_KILL_GET
	if (LINUX_KERNEL_MMR (2, 6, 32)) {
		const char *value;

		rc = prctl (PR_MCE_KILL_GET, 0, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (rc) {
			case PR_MCE_KILL_DEFAULT:
				value = "system default";
				break;
			case PR_MCE_KILL_EARLY:
				value = "early kill";
				break;
			case PR_MCE_KILL_LATE:
				value = "late kill";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("machine-check exception", "%s", value);
	}
#endif

#ifdef PR_GET_NO_NEW_PRIVS
	if (LINUX_KERNEL_MM (3, 5)) {
		const char *value;

		rc = prctl (PR_GET_NO_NEW_PRIVS, 0, 0, 0, 0);
		if (rc < 0)
			value = UNKNOWN_STR;
		else {
			switch (rc) {
			case 0:
				value = "normal execve";
				break;
			case 1:
				value = "enabled";
				break;
			default:
				value = UNKNOWN_STR;
				break;
			}
		}
		entry ("no new privileges", "%s", value);
	}
#endif

#ifdef PR_GET_TIMERSLACK
	if (LINUX_KERNEL_MMR (2, 6, 28)) {
		rc = prctl (PR_GET_TIMERSLACK, 0, 0, 0, 0);
		if (rc < 0)
			entry ("timer slack", "%s", UNKNOWN_STR);
		else
			entry ("timer slack", "%dns", rc);
	}
#endif

#ifdef PR_GET_CHILD_SUBREAPER
	if (LINUX_KERNEL_MM (3, 4)) {
		rc = prctl (PR_GET_CHILD_SUBREAPER, &arg2, 0, 0, 0);
		if (rc < 0)
			entry ("child subreaper", "%s", UNKNOWN_STR);
		else
			entry ("child subreaper", "%s", arg2 ? YES_STR : NO_STR);
	}
#endif

#ifdef PR_GET_TID_ADDRESS
	rc = prctl (PR_GET_TID_ADDRESS, &arg2, 0, 0, 0);
	if (rc < 0)
		entry ("clear child tid address", "%s", UNKNOWN_STR);
	else
		entry ("clear child tid address", "%p", arg2);
#endif
}

static void
handle_proc_branch_linux (void)
{
	char    buffer[1024];
	char    path[PATH_MAX];
	char    name[16];
	char    pid[16];
	char    ppid[16];
	size_t  len;
	char   *p;
	FILE   *f;
	char   *str = NULL;

	sprintf (pid, "%d", (int)getpid ());

	/* This is one God-awful interface */
	while (true) {
		sprintf (path, "/proc/%s/status", pid);

		f = fopen (path, "r");
		if (! f) {
			appendf (&str, "%s", UNKNOWN_STR);
			goto out;
		}

		while (fgets (buffer, sizeof (buffer), f)) {
			len = strlen (buffer);
			buffer[len-1] = '\0';

			if ((p=strstr (buffer, "Name:")) == buffer) {
				p += 1+strlen ("Name:"); /* jump over tab char */
				sprintf (name, "%s", p);
			}

			if ((p=strstr (buffer, "PPid:")) == buffer) {
				p += 1+strlen ("PPid:"); /* jump over tab char */
				sprintf (ppid, "%s", p);

				/* got all we need now */
				break;
			}
		}

		fclose (f);

		/* ultimate parent == PID 1 == '/sbin/init' */
		if (! strcmp (pid, "1")) {
			appendf (&str, "%s ('%s')", pid, name);
			break;
		} else {
			appendf (&str, "%s ('%s'), ", pid, name);
		}

		/* parent is now the pid to search for */
		sprintf (pid, "%s", ppid);
	}
out:

	entry ("ancestry", "%s", str);
	free (str);
}

static void
show_security_module_context_linux (void)
{
	char   *context = NULL;
	char   *mode = NULL;

#if defined (HAVE_APPARMOR)
	if (aa_is_enabled ()) {
		/* XXX: The mode string is *NOT* be freed since it forms
		 * part of the allocation returned in context.
		 *
		 * See aa_gettaskcon(2) for details.
		 */
		if (aa_gettaskcon (user.pid, &context, &mode) < 0)
			die ("failed to query AppArmor context");
	}
#endif

#if defined (HAVE_SELINUX_SELINUX_H)
	if (is_selinux_enabled ()) {
		if (getpidcon (user.pid, &context) < 0)
			die ("failed to query SELinux context");
	}
#endif
	if (context) {
		if (mode) {
			entry ("context", "%s (%s)", context, mode);
		} else {
			entry ("context", "%s", context);
		}
	} else {
		entry ("context", "%s", UNKNOWN_STR);
	}

    free (context);
}

static void
show_security_module_linux (void)
{
	char *lsm = UNKNOWN_STR;

#if defined (HAVE_APPARMOR)
	if (aa_is_enabled ())
		lsm = "AppArmor";
#endif

#if defined (HAVE_SELINUX_SELINUX_H)
	if (is_selinux_enabled () == 1) {

		if (is_selinux_mls_enabled () == 1)
			lsm = "SELinux (MLS)";
		else
			lsm = "SELinux";
	}
#endif

	entry ("name", "%s", lsm);

    show_security_module_context_linux ();
}

static void
show_semaphores_linux (void)
{
	int               i;
	int               id;
	int               max;
	struct semid_ds   semid_ds;
	struct seminfo    info;
	struct ipc_perm  *perm;
	char              formatted_otime[CTIME_BUFFER];
	char              formatted_ctime[CTIME_BUFFER];
	char             *modestr = NULL;
	union semun       arg;

	arg.array = (unsigned short int *)(void *)&info;
	max = semctl (0, 0, SEM_INFO, arg);
	if (max < 0)
		goto out;

	section_open ("info");

	entry ("semmap", "%d", info.semmap);
	entry ("semmni", "%d", info.semmni);
	entry ("semmns", "%d", info.semmns);
	entry ("semmnu", "%d", info.semmnu);
	entry ("semmsl", "%d", info.semmsl);
	entry ("semopm", "%d", info.semopm);
	entry ("semume", "%d", info.semume);
	entry ("semusz", "%d", info.semusz);
	entry ("semvmx", "%d", info.semvmx);
	entry ("semaem", "%d", info.semaem);

	section_close ();

	container_open ("set");

	object_open (false);

	for (i = 0; i <= max; i++) {
		char *id_str = NULL;

		/* see semctl(2) */
		arg.buf = (struct semid_ds *)&semid_ds;

		id = semctl (i, 0, SEM_STAT, arg);
		if (id < 0) {
			/* found an unused slot, so ignore it */
			continue;
		}

		perm = &semid_ds.sem_perm;

		modestr = format_perms (perm->mode);
		if (! modestr)
			die ("failed to allocate space for permissions string");

		/* May not have been set */
		if (semid_ds.sem_otime)
			format_time (&semid_ds.sem_otime, formatted_otime, sizeof (formatted_otime));
		else
			sprintf (formatted_otime, "%s", NA_STR);

		format_time (&semid_ds.sem_ctime, formatted_ctime, sizeof (formatted_ctime));

		appendf (&id_str, "%d", id);

		container_open (id_str);
		free (id_str);

		object_open (false);

		/* pad out to max pointer size represented in hex.
		*/
		entry ("key", "0x%.*x", POINTER_SIZE * 2, perm->__key);
		entry ("sequence", "%u", perm->__seq);

		entry ("number in set", "%lu", semid_ds.sem_nsems);

		section_open ("permissions");
		entry ("octal", "%4.4o", perm->mode);
		entry ("symbolic", "%s", modestr);
		free (modestr);
		section_close ();

		section_open ("creator");
		entry ("euid", "%u ('%s')", perm->cuid, get_user_name (perm->cuid));
		entry ("egid", "%u ('%s')", perm->cgid, get_group_name (perm->cgid));
		section_close ();

		section_open ("owner");
		entry ("uid", "%u ('%s')", perm->uid, get_user_name (perm->uid));
		entry ("gid", "%u ('%s')", perm->gid, get_group_name (perm->gid));
		section_close ();

		section_open ("times");
		entry ("last semop (otime)", "%lu (%s)", semid_ds.sem_otime, formatted_otime);
		entry ("last change (ctime)", "%lu (%s)", semid_ds.sem_ctime, formatted_ctime);
		section_close ();

		object_close (false);

		container_close ();
	}

	object_close (false);

	container_close ();

out:
    /* compiler appeasement */
    return;
}

static void
show_shared_mem_linux (void)
{
	int               i;
	int               id;
	int               max;
	struct shm_info   info;
	struct shmid_ds   shmid_ds;
	struct ipc_perm  *perm;
	char              formatted_atime[CTIME_BUFFER];
	char              formatted_ctime[CTIME_BUFFER];
	char              formatted_dtime[CTIME_BUFFER];
	char             *modestr = NULL;
	int               locked = -1;
	int               destroy = -1;
	char             *cpid = NULL;
	char             *lpid = NULL;

	max = shmctl (0, SHM_INFO, (void *)&info);
	if (max < 0)
		goto out;

	/* Display summary details */

	section_open ("info");

	entry ("segments", "%u", info.used_ids);
	entry ("pages", "%lu", info.shm_tot);
	entry ("shm_rss", "%lu", info.shm_rss);
	entry ("shm_swp", "%lu", info.shm_swp);

	/* Apparently unused */
	entry ("swap_attempts", "%lu", info.swap_attempts);
	entry ("swap_successes", "%lu", info.swap_successes);

	section_close ();

	container_open ("segments");

	object_open (false);

	for (i = 0; i <= max; i++) {
		char *id_str = NULL;

		id = shmctl (i, SHM_STAT, &shmid_ds);
		if (id < 0) {
			/* found an unused slot, so ignore it */
			continue;
		}

		perm = &shmid_ds.shm_perm;

		modestr = format_perms (perm->mode);
		if (! modestr)
			die ("failed to allocate space for permissions string");

		locked = (perm->mode & SHM_LOCKED);
		destroy = (perm->mode & SHM_DEST);

		format_time (&shmid_ds.shm_atime, formatted_atime, sizeof (formatted_atime));
		format_time (&shmid_ds.shm_ctime, formatted_ctime, sizeof (formatted_ctime));
		format_time (&shmid_ds.shm_dtime, formatted_dtime, sizeof (formatted_dtime));

		cpid = pid_to_name (shmid_ds.shm_cpid);
		lpid = pid_to_name (shmid_ds.shm_lpid);

		appendf (&id_str, "%d", id);

		container_open (id_str);
		free (id_str);

		object_open (false);

		/* pad out to max pointer size represented in hex.
		*/
		entry ("key", "0x%.*x", POINTER_SIZE * 2, perm->__key);
		entry ("sequence", "%u", perm->__seq);

		section_open ("permissions");
		entry ("octal", "%4.4o", perm->mode);
		entry ("symbolic", "%s", modestr);
		section_close ();

		section_open ("pids");
		entry ("create", "%d (%s)", shmid_ds.shm_cpid, cpid ? cpid : UNKNOWN_STR);
		entry ("last", "%d (%s)", shmid_ds.shm_lpid, lpid ? lpid : UNKNOWN_STR);
		section_close ();

		entry ("attachers", "%lu", shmid_ds.shm_nattch);

		section_open ("creator");
		entry ("euid", "%u ('%s')", perm->cuid, get_user_name (perm->cuid));
		entry ("egid", "%u ('%s')", perm->cgid, get_group_name (perm->cgid));
		section_close ();

		section_open ("owner");
		entry ("uid", "%u ('%s')", perm->uid, get_user_name (perm->uid));
		entry ("gid", "%u ('%s')", perm->gid, get_group_name (perm->gid));
		section_close ();

		entry ("segment size", "%lu", shmid_ds.shm_segsz);

		section_open ("times");
		entry ("last attach (atime)", "%lu (%s)", shmid_ds.shm_atime, formatted_atime);
		entry ("last detach (dtime)", "%lu (%s)", shmid_ds.shm_dtime, formatted_dtime);
		entry ("last change (ctime)", "%lu (%s)", shmid_ds.shm_ctime, formatted_ctime);
		section_close ();

		entry ("locked", "%s", locked == 0 ? NO_STR
				: locked > 0 ? YES_STR
				: NA_STR);
		entry ("destroy", "%s", destroy == 0 ? NO_STR
				: destroy > 0 ? YES_STR
				: NA_STR);

		object_close (false);

		container_close ();

		free (modestr);
		if (cpid)
			free (cpid);
		if (lpid)
			free (lpid);
	}

	object_close (false);

	container_close ();

out:
    /* compiler appeasement */
    return;
}

static void
show_timezone_linux (void)
{
	tzset ();

	entry ("tzname[0]", "'%s'", tzname[0]);
	entry ("tzname[1]", "'%s'", tzname[1]);
	entry ("timezone", "%ld", timezone);
	entry ("daylight", "%d", daylight);
}	

static const char *
get_ioprio_class_name (int ioprio)
{
	struct procenv_map *p;

	for (p = io_priorities_class_map; p && p->name; p++) {
		if (ioprio == p->num)
			return p->name;
	}

	return NULL;
}

/* No GLib wrapper, so create one */
static int
io_prio_get (int which, int who)
{
    return syscall(SYS_ioprio_get, which, who);
}

static void
get_io_priorities_linux (struct procenv_priority *iop)
{
	iop->process = io_prio_get (IOPRIO_WHO_PROCESS, 0);
	iop->pgrp    = io_prio_get (IOPRIO_WHO_PGRP   , 0);
	iop->user    = io_prio_get (IOPRIO_WHO_USER   , 0);
}

static void
show_io_priorities_linux (void)
{
	section_open ("process");
	entry ("class", "%s", get_ioprio_class_name (IOPRIO_PRIO_CLASS (priority_io.process)));
	entry ("priority", "%d", IOPRIO_PRIO_DATA (priority_io.process));
	section_close ();

	section_open ("group");
	entry ("class", "%s", get_ioprio_class_name (IOPRIO_PRIO_CLASS (priority_io.pgrp)));
	entry ("priority", "%d", IOPRIO_PRIO_DATA (priority_io.pgrp));
	section_close ();

	section_open ("user");
	entry ("class", "%s", get_ioprio_class_name (IOPRIO_PRIO_CLASS (priority_io.user)));
	entry ("priority", "%d", IOPRIO_PRIO_DATA (priority_io.user));
	section_close ();
}

static void
show_rlimits_linux (void)
{
	show_limit (RLIMIT_AS);
	show_limit (RLIMIT_CORE);
	show_limit (RLIMIT_CPU);
	show_limit (RLIMIT_DATA);
	show_limit (RLIMIT_FSIZE);

#if defined (RLIMIT_RTTIME)
	if (LINUX_KERNEL_MMR (2, 6, 25)) {
		show_limit (RLIMIT_RTTIME);
	}
#endif

	show_limit (RLIMIT_LOCKS);
	show_limit (RLIMIT_MEMLOCK);

#if defined (RLIMIT_MSGQUEUE)
	if (LINUX_KERNEL_MMR (2, 6, 8)) {
		show_limit (RLIMIT_MSGQUEUE);
	}
#endif

#if defined RLIMIT_NICE
	if (LINUX_KERNEL_MMR (2, 6, 12)) {
		show_limit (RLIMIT_NICE);
	}
#endif

	show_limit (RLIMIT_NOFILE);
	show_limit (RLIMIT_NPROC);
	show_limit (RLIMIT_RSS);
	show_limit (RLIMIT_RTPRIO);

#if defined (RLIMIT_SIGPENDING)
	if (LINUX_KERNEL_MMR (2, 6, 8)) {
	show_limit (RLIMIT_SIGPENDING);
	}
#endif

	show_limit (RLIMIT_STACK);
}

static void
get_proc_name_linux (struct procenv_user *user)
{
    assert (user);

	if (LINUX_KERNEL_MMR (2, 6, 11)) {
		if (prctl (PR_GET_NAME, user->proc_name, 0, 0, 0) < 0)
			strcpy (user->proc_name, UNKNOWN_STR);
	}
}

#if defined (HAVE_NUMA_H)

static struct procenv_map numa_mempolicy_map[] = {
	mk_map_entry (MPOL_DEFAULT),
	mk_map_entry (MPOL_PREFERRED),
	mk_map_entry (MPOL_BIND),
	mk_map_entry (MPOL_INTERLEAVE),
};

static const char *
get_numa_policy (int policy)
{
	struct procenv_map *p;

	for (p = numa_mempolicy_map; p && p->name; p++) {
		if (p->num == policy)
			return p->name;
	}

	return NULL;
}
#endif /* HAVE_NUMA_H */

static void
handle_numa_memory_linux (void)
{
#if defined (HAVE_NUMA_H)
	int              policy;
	const char      *policy_name;
	char            *allowed_list = NULL;
	int              ret;
	unsigned long    node;
	unsigned long    allowed_size;

#if defined (HAVE_NUMA_H)
#if LIBNUMA_API_VERSION == 2
	struct bitmask  *allowed;
#else
	nodemask_t       allowed;
#endif
#endif

	/* part of the libnuma public API - stop the library calling
	 * exit(3) on error.
	 */
	numa_exit_on_error = 0;

	/* true if any numa nodes have been displayed yet */
	int              displayed = false;

	/* Number of numa nodes in *current* range */
	size_t           count = 0;

	/* Only valid to read these when count is >0 */
	size_t           last = 0;
	size_t           first = 0;
#endif /* HAVE_NUMA_H */

	header ("numa");

#if defined (HAVE_NUMA_H)
	if (numa_available () < 0)
		/* NUMA not supported on this system */
		goto out;

#if LIBNUMA_API_VERSION == 2
	entry ("api version", "%d", 2);
#else
	entry ("api version", "%d", 1);
#endif

	ret = get_mempolicy (&policy, NULL, 0, 0, 0);

	if (ret < 0) {
		entry ("policy", "%s", UNKNOWN_STR);
		goto out;
	}

	policy_name = get_numa_policy (policy);

	entry ("policy", "%s", policy_name ? policy_name : UNKNOWN_STR);

#if LIBNUMA_API_VERSION == 2
	entry ("maximum nodes", "%d", numa_num_possible_nodes ());
	entry ("configured nodes", "%d", numa_num_configured_nodes ());

	allowed = numa_get_mems_allowed ();
	if (! allowed)
		die ("failed to query NUMA allowed list");

	allowed_size = allowed->size;

#else
	entry ("maximum nodes", "%s", UNKNOWN_STR);
	entry ("configured nodes", "%d", numa_max_node ());

	allowed = numa_get_run_node_mask ();
	allowed_size = NUMA_NUM_NODES;
#endif

	for (node = 0; node < allowed_size; node++) {
		if (PROCENV_NUMA_BITMASK_ISSET (allowed, node)) {
			/* Record first entry in the range */
			if (! count)
				first = node;

			last = node;
			count++;
		} else {
			if (count) {
				if (first == last) {
					appendf (&allowed_list, "%s%d",
							displayed ? "," : "",
							first);
				} else {
					appendf (&allowed_list, "%s%d-%d",
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
			appendf (&allowed_list, "%s%d",
					displayed ? "," : "",
					first);
		} else {
			appendf (&allowed_list, "%s%d-%d",
					displayed ? "," : "",
					first, last);
		}
	}

	entry ("allowed list", "%s", allowed_list);

#if LIBNUMA_API_VERSION == 2
	numa_free_nodemask (allowed);
#endif

	free (allowed_list);

out:
#endif /* HAVE_NUMA_H */
	footer ();
}

/**
 * linux_kernel_version:
 *
 * @major: major kernel version number,
 * @minor: minor kernel version number,
 * @revision: kernel revision version,
 *
 * @minor and @revision may be -1 to denote that those version
 * elements are not important to the caller. Once a parameter
 * has been specified as -1, subsequent parameters are ignored
 * (treated as -1 too).
 *
 * Returns: true if running Linux kernel is atleast at version
 * specified by (@major, @minor, @revision), else false.
 **/
static bool
linux_kernel_version (int major, int minor, int revision)
{
	int  actual_version    = 0x000000;
	int  requested_version = 0x000000;
	int  actual_major      = -1;
	int  actual_minor      = -1;
	int  actual_revision   = -1;
	int  ret;

	assert (uts.release);
	assert (sizeof (int) >= 4);

	/* We need something to work with */
	assert (major > 0);

	ret = sscanf (uts.release, "%d.%d.%d",
			&actual_major, &actual_minor,
			&actual_revision);

	/* We need something to compare against */
	assert (ret && actual_major != -1);

	requested_version |= (0xFF0000 & (major << 16)); 

	if (minor != -1) {
		requested_version |= (0x00FF00 & (minor << 8));

		if (revision != -1)
			requested_version |= (0x0000FF & revision);
	}

	if (actual_revision != -1) {
		actual_version |= (0x0000FF & actual_revision);
	}

	if (actual_minor != -1)
		actual_version |= (0x00FF00 & (actual_minor << 8));

	if (actual_major != -1)
		actual_version |= (0xFF0000 & (actual_major << 16)); 


	if (actual_version >= requested_version)
		return true;

	return false;
}

#if defined (HAVE_SYS_CAPABILITY_H)
static int
get_capability_by_flag_type (cap_t cap_p, cap_flag_t type, cap_value_t cap)
{
    int               ret;
    cap_flag_value_t  result;

    assert (cap_p);

    ret = cap_get_flag (cap_p, cap, type, &result);

    return ret < 0 ? ret : result;
}
#endif /* HAVE_SYS_CAPABILITY_H */

#if defined (HAVE_SYS_CAPABILITY_H)
#ifdef PROCENV_NEED_LOCAL_CAP_GET_BOUND

static int
cap_get_bound (cap_value_t cap)
{
#if defined (PR_CAPBSET_READ)
	return prctl (PR_CAPBSET_READ, cap);
#else
	return -1;
#endif
}

#endif /* PROCENV_NEED_LOCAL_CAP_GET_BOUND */
#endif /* HAVE_SYS_CAPABILITY_H */

static const char *
get_scheduler_name (int sched)
{
	struct procenv_map *p;

	for (p = scheduler_map; p && p->name; p++) {
		if (p->num == sched)
			return p->name;
	}

	return NULL;
}

static void
get_tty_locked_status_linux (struct termios *lock_status)
{
	assert (lock_status);
	assert (user.tty_fd != -1);

	if (ioctl (user.tty_fd, TIOCGLCKTRMIOS, lock_status) < 0) {
		/* Set to unlocked */
		memset (lock_status, '\0', sizeof (struct termios));
	}
}

static void
handle_scheduler_type_linux (void)
{
	int sched;

	sched = sched_getscheduler (0);

	entry ("type", "%s",
			sched < 0 ? UNKNOWN_STR :
			get_scheduler_name (sched));
}

static void
show_extended_if_flags_linux (const char *interface, unsigned short *flags)
{
	int                        sock;
	struct ifreq               ifr;
	const struct procenv_map  *p;

	assert (interface);
	assert (flags);

	/* We need to create a socket to query an interfaces mac
	 * address. Don't ask me why...
	 */
	sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (sock < 0)
		return;

	memset (&ifr, 0, sizeof (struct ifreq));
	strncpy (ifr.ifr_name, interface, IFNAMSIZ-1);

	if (ioctl (sock, SIOCGIFPFLAGS, &ifr) < 0)
		goto out;

	*flags = ifr.ifr_flags;

	for (p = if_extended_flag_map_linux; p && p->name; p++) {
		if (*flags & p->num) {
			object_open (false);
			entry (p->name, "0x%x", p->num);
			object_close (false);
		}
	}
out:
	close (sock);
}

static PROCENV_CPU_SET_TYPE *
get_cpuset_linux (void)
{
	int ret = 0;

#if ! defined (CPU_ALLOC)
	/* RHEL 5 */
	static PROCENV_CPU_SET_TYPE  cpu_set_real;
#else
	long   max;
#endif

	size_t size;
	PROCENV_CPU_SET_TYPE *cs = NULL;

#if defined (CPU_ALLOC)

	max = get_sysconf (_SC_NPROCESSORS_ONLN);
	if (max < 0)
		return NULL;

	cs = CPU_ALLOC (max);
	if (! cs)
		return NULL;

	size = CPU_ALLOC_SIZE (max);
	CPU_ZERO_S (size, cs);

#else /* ! CPU_ALLOC */

	size = sizeof (PROCENV_CPU_SET_TYPE);

	CPU_ZERO (&cpu_set_real);
	cs = &cpu_set_real;

#endif /* CPU_ALLOC */

	/* We could use sched_getaffinity(2) rather than
	 * sched_getaffinity() on Linux (2.5.8+) but
	 * pthread_getaffinity_np() provides the same information...
	 * Except it is missing on kFreeBSD systems (!) so we have to
	 * use sched_getaffinity() there. :(
	 */
#if ! defined (CPU_ALLOC)
	ret = sched_getaffinity (0, size, cs);
#else

	/* On a hyperthreaded system, "size" as above may not actually
	 * be big enough, and we get EINVAL (hwloc has a similar
	 * workaround).
	 */
	{
		int mult = 0;
		while ((ret = pthread_getaffinity_np (pthread_self (), size, cs))
				== EINVAL) {
			CPU_FREE (cs);
			/* Bail out at an arbitrary value.  */
			if (mult > 128) break;
			mult += 2;
			cs = CPU_ALLOC (mult * max);
			size = CPU_ALLOC_SIZE (mult * max);
			CPU_ZERO_S (size, cs);
		}
	}
#endif
	if (ret)
		goto err;

	return cs;
err:
#if defined (CPU_ALLOC)
	CPU_FREE (cs);
#endif
	return NULL;
}

static void
free_cpuset_linux (PROCENV_CPU_SET_TYPE *cs)
{
#if defined (CPU_ALLOC)
	CPU_FREE (cs);
#endif
}

bool cpuset_has_cpu_linux (const PROCENV_CPU_SET_TYPE *cs,
		PROCENV_CPU_TYPE cpu)
{
	return CPU_ISSET (cpu, cs);
}

struct procenv_ops platform_ops =
{
	.driver                        = PROCENV_SET_DRIVER (linux),

	.get_cpuset                    = get_cpuset_linux,
	.free_cpuset                   = free_cpuset_linux,
	.cpuset_has_cpu                = cpuset_has_cpu_linux,
	.get_user_misc                 = get_user_misc_linux,
	.get_proc_name                 = get_proc_name_linux,
	.get_io_priorities             = get_io_priorities_linux,
	.get_mtu                       = get_mtu_generic,

	.get_tty_locked_status         = get_tty_locked_status_linux,
	.get_kernel_bits               = get_kernel_bits_generic,

	.signal_map                    = signal_map_linux,
	.if_flag_map                   = if_flag_map_linux,
	.personality_map               = personality_map_linux,
	.personality_flag_map          = personality_flag_map_linux,

	.show_capabilities             = show_capabilities_linux,
	.show_cgroups                  = show_cgroups_linux,
	.show_confstrs                 = show_confstrs_generic,

	.show_cpu_affinities           = show_cpu_affinities_generic,

	.show_cpu                      = show_cpu_linux,
	.show_extended_if_flags        = show_extended_if_flags_linux,
	.show_fds                      = show_fds_linux,
	.show_io_priorities            = show_io_priorities_linux,
	.show_mounts                   = show_mounts_generic_linux,
	.show_msg_queues               = show_msg_queues_linux,
	.show_namespaces               = show_namespaces_linux,
	.show_oom                      = show_oom_linux,
	.show_prctl                    = show_prctl_linux,
	.show_rlimits                  = show_rlimits_linux,
	.show_security_module          = show_security_module_linux,
	.show_semaphores               = show_semaphores_linux,
	.show_shared_mem               = show_shared_mem_linux,
	.show_timezone                 = show_timezone_linux,

	.handle_numa_memory            = handle_numa_memory_linux,
	.handle_proc_branch            = handle_proc_branch_linux,
	.handle_scheduler_type         = handle_scheduler_type_linux,

};
