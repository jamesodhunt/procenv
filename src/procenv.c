/*--------------------------------------------------------------------
 * Description: Simple program to display information on process
 *              environment.
 *
 * Date: 24 October 2012.
 *
 * Author: James Hunt <james.hunt@ubuntu.com>.
 *
 * Licence: GPLv3. See below...
 *--------------------------------------------------------------------
 *
 * Copyright © 2012 James Hunt.
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

#include <procenv.h>

extern char **environ;

/**
 * output:
 *
 * Where to send output.
 **/
Output output = OUTPUT_STDOUT;

/**
 * output_file:
 *
 * Name or output file to send output to if not NULL.
 **/
const char *output_file = NULL;

/**
 * output_fd:
 *
 * File descriptor associated with output_file.
 **/
int output_fd = -1;

/**
 * reexec:
 *
 * TRUE if we should re-exec at the end.
 **/
int reexec = FALSE;

/**
 * selected_option:
 *
 * A non-zero value denotes user has requested a subset of the available
 * information with the value representing the short command-line
 * option.
 **/
int selected_option = 0;

/**
 * indent:
 *
 * Number of spaces to indent output
 **/
int indent = 0;

/**
 * program_name:
 *
 * Name of program.
 **/
const char *program_name;

/**
 * exec_args:
 *
 * Arguments used for re-exec'ing ourselves.
 **/
char **exec_args = NULL;

struct procenv_user     user;
struct procenv_misc     misc;
struct procenv_priority priority;

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
struct mntopt_map {
	uint64_t   flag;
	char      *name;
} mntopt_map[] = {

	{ MNT_ACLS         , "acls" },
	{ MNT_ASYNC        , "asynchronous" },
	{ MNT_EXPORTED     , "NFS-exported" },
	{ MNT_GJOURNAL     , "gjournal" },
	{ MNT_LOCAL        , "local" },
	{ MNT_MULTILABEL   , "multilabel" },
#ifndef __FreeBSD_kernel__
	{ MNT_NFS4ACLS     , "nfsv4acls" },
#endif
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
#ifndef __FreeBSD_kernel__
	{ MNT_SUJ          , "journaled soft-updates" },
#endif
	{ MNT_SYNCHRONOUS  , "synchronous" },
	{ MNT_UNION        , "union" },

	{ 0, NULL },
};
#endif

struct procenv_map output_map[] = {
	{ OUTPUT_FILE   , "file" },
	{ OUTPUT_STDERR , "stderr" },
	{ OUTPUT_STDOUT , "stdout" },
	{ OUTPUT_SYSLOG , "syslog" },
	{ OUTPUT_TERM   , "terminal" }
};

struct baud_speed baud_speeds[] = {
    SPEED (B0),
    SPEED (B50),
    SPEED (B75),
    SPEED (B110),
    SPEED (B134),
    SPEED (B150),
    SPEED (B200),
    SPEED (B300),
    SPEED (B600),
    SPEED (B1200),
    SPEED (B1800),
    SPEED (B2400),
    SPEED (B4800),
    SPEED (B9600),
    SPEED (B19200),
    SPEED (B38400),
    SPEED (B57600),
    SPEED (B115200),
    SPEED (B230400),

    /* terminator */
    { 0, NULL }
};

/* Really, every single sysconf variable should be ifdef'ed since it
 * may not exist on a particular system, but that makes the code look
 * untidy.
 *
 * The ifdefs we have below seem sufficient for modern systems
 * (Ubuntu Lucid and newer). If only we could use ifdef in macros.
 * sigh...
 */
struct procenv_map sysconf_map[] = {
	mk_posix_sysconf_map_entry (ARG_MAX),
	mk_posix_sysconf_map_entry (BC_BASE_MAX),
	mk_posix_sysconf_map_entry (BC_DIM_MAX),
	mk_posix_sysconf_map_entry (BC_SCALE_MAX),
	mk_posix_sysconf_map_entry (BC_STRING_MAX),
	mk_posix_sysconf_map_entry (CHILD_MAX),
	mk_sysconf_map_entry (_SC_CLK_TCK),
	mk_posix_sysconf_map_entry (COLL_WEIGHTS_MAX),
	mk_posix_sysconf_map_entry (EXPR_NEST_MAX),
	mk_posix_sysconf_map_entry (HOST_NAME_MAX),
	mk_posix_sysconf_map_entry (LINE_MAX),
	mk_posix_sysconf_map_entry (LOGIN_NAME_MAX),
	mk_posix_sysconf_map_entry (OPEN_MAX),
	mk_posix_sysconf_map_entry (PAGESIZE),
	mk_posix_sysconf_map_entry (RE_DUP_MAX),
	mk_posix_sysconf_map_entry (STREAM_MAX),
	mk_posix_sysconf_map_entry (SYMLOOP_MAX),
	mk_posix_sysconf_map_entry (TTY_NAME_MAX),
	mk_posix_sysconf_map_entry (TZNAME_MAX),
	{ _SC_VERSION, "_POSIX_VERSION(_SC_VERSION)" },
#ifdef _SC_POSIX2_C_DEV
	mk_posix_sysconf_map_entry (POSIX2_C_DEV),
#endif
	mk_posix_sysconf_map_entry (BC_BASE_MAX),
	mk_posix_sysconf_map_entry (BC_DIM_MAX),
	mk_posix_sysconf_map_entry (BC_SCALE_MAX),
	mk_posix_sysconf_map_entry (BC_STRING_MAX),
	mk_posix_sysconf_map_entry (COLL_WEIGHTS_MAX),
	mk_posix_sysconf_map_entry (EXPR_NEST_MAX),
	mk_posix_sysconf_map_entry (LINE_MAX),
	mk_posix_sysconf_map_entry (RE_DUP_MAX),
	{ _SC_2_VERSION, "POSIX2_VERSION(_SC_2_VERSION)" },
	{ _SC_2_C_DEV, "POSIX2_C_DEV(_SC_2_C_DEV)" },
	{ _SC_2_FORT_DEV, "POSIX2_FORT_DEV(_SC_2_FORT_DEV)" },
	{ _SC_2_FORT_RUN, "POSIX2_FORT_RUN(_SC_2_FORT_RUN)" },
	{ _SC_2_LOCALEDEF, "_POSIX2_LOCALEDEF(_SC_2_LOCALEDEF)" },
	{ _SC_2_SW_DEV, "POSIX2_SW_DEV(_SC_2_SW_DEV)" },
	mk_sysconf_map_entry (_SC_PHYS_PAGES),
#if defined (_SC_AVPHYS_PAGES)
	mk_sysconf_map_entry (_SC_AVPHYS_PAGES),
#endif
	mk_sysconf_map_entry (_SC_NPROCESSORS_CONF),
	mk_sysconf_map_entry (_SC_NPROCESSORS_ONLN),

	mk_posixopt_sysconf_map_entry (ADVISORY_INFO),
	mk_posixopt_sysconf_map_entry (ASYNCHRONOUS_IO),
	mk_posixopt_sysconf_map_entry (BARRIERS),
	mk_sysconf_map_entry (_POSIX_CHOWN_RESTRICTED),
	mk_posixopt_sysconf_map_entry (CLOCK_SELECTION),
	mk_posixopt_sysconf_map_entry (CPUTIME),
	mk_posixopt_sysconf_map_entry (FILE_LOCKING),
	mk_posixopt_sysconf_map_entry (FSYNC),
	mk_posixopt_sysconf_map_entry (JOB_CONTROL),
	mk_posixopt_sysconf_map_entry (MAPPED_FILES),
	mk_posixopt_sysconf_map_entry (MEMLOCK),
	mk_posixopt_sysconf_map_entry (MEMLOCK_RANGE),
	mk_posixopt_sysconf_map_entry (MEMORY_PROTECTION),
	mk_posixopt_sysconf_map_entry (MESSAGE_PASSING),
	mk_posixopt_sysconf_map_entry (MONOTONIC_CLOCK),
#ifdef _SC_MULTI_PROCESS
	mk_posixopt_sysconf_map_entry (MULTI_PROCESS),
#endif
	mk_posixopt_sysconf_map_entry (PRIORITIZED_IO),
	mk_posixopt_sysconf_map_entry (PRIORITY_SCHEDULING),
	mk_sysconf_map_entry (_POSIX_RAW_SOCKETS),
	mk_posixopt_sysconf_map_entry (READER_WRITER_LOCKS),
	mk_posixopt_sysconf_map_entry (REALTIME_SIGNALS),
	mk_posixopt_sysconf_map_entry (REGEXP),
	mk_posixopt_sysconf_map_entry (SAVED_IDS),
	mk_posixopt_sysconf_map_entry (SEMAPHORES),
	mk_posixopt_sysconf_map_entry (SHARED_MEMORY_OBJECTS),
	mk_posixopt_sysconf_map_entry (SHELL),
	mk_posixopt_sysconf_map_entry (SPAWN),
	mk_posixopt_sysconf_map_entry (SPIN_LOCKS),
	mk_posixopt_sysconf_map_entry (SPORADIC_SERVER),
	mk_posixopt_sysconf_map_entry (SYNCHRONIZED_IO),
	mk_posixopt_sysconf_map_entry (THREAD_ATTR_STACKSIZE),
	mk_posixopt_sysconf_map_entry (THREAD_CPUTIME),
	mk_posixopt_sysconf_map_entry (THREAD_PRIO_INHERIT),
	mk_posixopt_sysconf_map_entry (THREAD_PRIO_PROTECT),
	mk_posixopt_sysconf_map_entry (THREAD_PRIORITY_SCHEDULING),
	mk_posixopt_sysconf_map_entry (THREAD_PROCESS_SHARED),
	mk_posixopt_sysconf_map_entry (THREAD_SAFE_FUNCTIONS),
	mk_posixopt_sysconf_map_entry (THREAD_SPORADIC_SERVER),
	mk_posixopt_sysconf_map_entry (THREADS),
	mk_posixopt_sysconf_map_entry (TIMEOUTS),
	mk_posixopt_sysconf_map_entry (TIMERS),
	mk_posixopt_sysconf_map_entry (TRACE),
	mk_posixopt_sysconf_map_entry (TRACE_EVENT_FILTER),
	mk_posixopt_sysconf_map_entry (TRACE_INHERIT),
	mk_posixopt_sysconf_map_entry (TRACE_LOG),
#ifdef _SC_TYPED_MEMORY_OBJECT
	mk_posixopt_sysconf_map_entry (TYPED_MEMORY_OBJECT),
#endif
	mk_sysconf_map_entry (_POSIX_VDISABLE),
	mk_sysconf_map_entry (_XOPEN_CRYPT),
	mk_sysconf_map_entry (_XOPEN_LEGACY),
	mk_sysconf_map_entry (_XOPEN_REALTIME),
	mk_sysconf_map_entry (_XOPEN_REALTIME_THREADS),
	mk_sysconf_map_entry (_XOPEN_UNIX),

	{ 0, NULL }
};

/* Signal numbers are different per architecture.
 *
 * This lookup table allows use to ignore the numbers and display nice
 * symbolic names and also to order by signal number (which values
 * change on different architectures).
 */
struct procenv_map signal_map[] = {

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
#if defined (PROCENV_LINUX)
	mk_map_entry (SIGIOT),
#endif

#if defined (PROCENV_LINUX)
	{SIGPOLL, "SIGPOLL|SIGIO" },
#endif

	mk_map_entry (SIGPROF),

#if defined (PROCENV_LINUX)
	mk_map_entry (SIGPWR),
	mk_map_entry (SIGSTKFLT),
#endif

	mk_map_entry (SIGSYS),

#if defined (PROCENV_LINUX)
	mk_map_entry (SIGUNUSED),
#endif
	mk_map_entry (SIGURG),
	mk_map_entry (SIGVTALRM),
	mk_map_entry (SIGWINCH),
	mk_map_entry (SIGXCPU),
	mk_map_entry (SIGXFSZ),

	{ 0, NULL },
};

struct procenv_map scheduler_map[] = {

	mk_map_entry (SCHED_OTHER),
	mk_map_entry (SCHED_FIFO),
	mk_map_entry (SCHED_RR),
#if defined (PROCENV_LINUX)
	mk_map_entry (SCHED_BATCH),
#ifdef SCHED_IDLE
	mk_map_entry (SCHED_IDLE),
#endif
#endif
		
	{ 0, NULL }
};

void
usage (void)
{
	show ("Usage: %s [option]", PACKAGE_STRING);
	show ("");
	show ("Description: Display process environment.");
	show ("");
	show ("Options:");
	show ("");
	show ("  -a, --meta          : Display meta details.");
	show ("  -b, --libs          : Display library details.");
	show ("  -c, --cgroup[s]     : Display cgroup details (Linux only).");
	show ("  -d, --compiler      : Display compiler details.");
	show ("  -e, --env[ironment] : Display environment variables.");
	show ("  --exec              : Treat non-option arguments as program to execute.");
	show ("  -f, --fds           : Display file descriptor details.");
	show ("  --file=<file>       : Send output to <file> (implies --output=file).");
	show ("  -g, --sizeof        : Display sizes of data types in bytes.");
	show ("  -h, --help          : This help text.");
	show ("  -i, --misc          : Display miscellaneous details.");
	show ("  -j, --uname         : Display uname details.");
	show ("  -k, --clock[s]      : Display clock details.");
	show ("  -l, --limits        : Display limits.");
	show ("  -m, --mount[s]      : Display mount details.");
	show ("  -n, --confstr       : Display confstr details.");
	show ("  -o, --oom           : Display out-of-memory manager details (Linux only)");
	show ("  --output=<type>     : Send output to alternative location. Type can be one of:");
	show ("");
	show ("                      file     # send output to a file.");
	show ("                      stderr   # write to standard error.");
	show ("                      stdout   # write to standard output (default).");
	show ("                      syslog   # write to the system log file.");
	show ("                      terminal # write to terminal.");
	show ("");
	show ("  -p, --proc[ess]     : Display process details.");
	show ("  -q, --time          : Display time details.");
	show ("  -r, --range[s]      : Display range of data types.");
	show ("  -s, --signal[s]     : Display signal details.");
	show ("  -t, --tty           : Display terminal details.");
	show ("  -u, --stat          : Display stat details.");
	show ("  -v, --version       : Display version details.");
	show ("  -w, --capabilities  : Display capaibility details (Linux only).");
	show ("  -x, --pathconf      : Display pathconf details.");
	show ("  -y, --sysconf       : Display sysconf details.");
	show ("  -z, --timezone      : Display timezone details.");
	show ("");
	show ("Notes:");
	show ("");
	show ("  - If no option is specified, all details are displayed.");
	show ("  - Only one option may be specified.");
	show ("");
}

void
show_pathconfs (ShowMountType what,
		const char *dir)
{
	assert (dir);

	if (what == SHOW_ALL)
		showi (INDENT, "pathconf for path '%s':", dir);
	else
		show ("pathconf for path '%s':", dir);

	show_pathconf (what, dir, _PC_LINK_MAX);
	show_pathconf (what, dir, _PC_MAX_CANON);
	show_pathconf (what, dir, _PC_MAX_INPUT);
	show_pathconf (what, dir, _PC_NAME_MAX);
	show_pathconf (what, dir, _PC_PATH_MAX);
	show_pathconf (what, dir, _PC_PIPE_BUF);
	show_pathconf (what, dir, _PC_CHOWN_RESTRICTED);
	show_pathconf (what, dir, _PC_NO_TRUNC);
	show_pathconf (what, dir, _PC_VDISABLE);
}

const char *
get_speed (speed_t speed)
{
    struct baud_speed *s;

    for (s = baud_speeds; s && s->name; s++) {
        if (speed == s->speed)
            return s->name;
    }

    return NULL;
}

/**
 * _show:
 *
 * @prefix: string prefix to write,
 * @indent: number of spaces to indent output to,
 * @fmt: printf-style format with associated arguments.
 *
 * Write output to @string, indented by @indent spaces.
 * Note that error scenarios cannot call die() as by definition output
 * may not be possible.
 **/
void
_show (const char *prefix, int indent, const char *fmt, ...)
{
	int       ret;
	va_list   ap;
	char     *buffer = NULL;
	int       fd;

	assert (fmt);

	buffer = strdup ("");
	assert (buffer);

	if (indent)
		appendf (&buffer, "%*s", indent, " ");

	if (prefix && *prefix)
		appendf (&buffer, "%s: ", prefix);

	va_start (ap, fmt);
	appendva (&buffer, fmt, ap);
	va_end (ap);

	appendf (&buffer, "\n");

	switch (output) {
	case OUTPUT_SYSLOG:
		syslog (LOG_INFO, "%s", buffer);
		ret = 0;
		break;

	case OUTPUT_STDOUT:
		ret = fputs (buffer, stdout);
		break;

	case OUTPUT_STDERR:
		ret = fputs (buffer, stderr);
		break;

	case OUTPUT_TERM:
		assert (user.tty_fd != -1);
		ret = write (user.tty_fd, buffer, strlen (buffer));
		if (ret < 0) {
			fprintf (stderr, "ERROR: failed to write to terminal\n");
			exit (EXIT_FAILURE);
		}
		break;

	case OUTPUT_FILE:
		assert (output_file);
		if (output_fd < 0) {
			output_fd = open (output_file,
					(O_WRONLY|O_CREAT),
					(S_IRWXU|S_IRGRP|S_IROTH));
			if (output_fd < 0) {
				fprintf (stderr, "ERROR: failed to open file '%s'\n",
						output_file);
				exit (EXIT_FAILURE);
			}
		}
		ret = write (output_fd, buffer, strlen (buffer));
		if (ret < 0) {
			fprintf (stderr, "ERROR: failed to write to file '%s'\n",
					output_file);
			exit (EXIT_FAILURE);
		}
		break;

	default:
		fprintf (stderr, "ERROR: invalid output type: %d\n", output);
		exit (EXIT_FAILURE);
		break;
	}

	if (ret < 0)
		goto error;

	free (buffer);

	return;

error:
	if (buffer)
		free (buffer);

	fprintf (stderr, "ERROR: failed to construct message\n");
	exit (EXIT_FAILURE);
}

/**
 * header:
 * @fmt: printf-style format with associated arguments.
 *
 * Display a header to stdout, unless user has requested
 * a summary view.
 */
void
header (const char *fmt, ...)
{
	char     buffer[PROCENV_BUFFER];
	va_list  ap;

	/* Don't display header if user has specified what to display as
	 * only 1 group of information will be displayed (so a header is
	 * unnecessary).
	 */
	if (selected_option)
		return;

	/* append colon */
	va_start (ap, fmt);
	vsprintf (buffer, fmt, ap);
	strncat (buffer, ":", sizeof (buffer));
	va_end (ap);

	_show ("", 0, buffer);
}

/**
 * fd_valid:
 * @fd: file descriptor.
 *
 * Return 1 if @fd is valid, else 0.
 **/
int
fd_valid (int fd)
{
	int flags = 0;

	if (fd < 0)
		return 0;

	errno = 0;
	flags = fcntl (fd, F_GETFL);

	if (flags < 0)
		return 0;

	/* redundant really */
	if (errno == EBADF)
		return 0;

	return 1;
}

/**
 * show_signals:
 *
 * Display signal dispositions.
 *
 * Note that to traditionalists, it might _appear_ pointless to
 * display whether a signal is ignored, but on Linux that is not
 * necessarily so...
 *
 * Under "Classical Unix":
 *
 * - across a fork(), a child inherits the parents
 *   signal mask *AND* dispositions.
 *
 * - across an exec*(3), a process inherits the original processes
 *   signal mask *ONLY*.
 *
 * Under Linux:
 *
 * In additional to the classical semantics, by careful use of clone(2),
 * it is possible for a child to inherit its parents dispositions
 * (using clones CLONE_SIGHAND+CLONE_VM flags). This is possible since
 * the child then shares the parents signal handlers, which inherantly
 * therefore provide access to the dispositions).
 **/
void
show_signals (void)
{
	int               i;
	int               rc;
	int               blocked;
	int               ignored;
	sigset_t          old_sigset;
	struct sigaction  act;

	header ("signals");

	/* Query blocked signals.
	 *
	 * How should be 0, but valgrind complains.
	 */
	if (sigprocmask (SIG_BLOCK, NULL, &old_sigset) < 0)
		die ("failed to query signal mask");

	for (i = 1; i <= NUM_SIGNALS; i++) {

		blocked = 0;
		ignored = 0;

		rc = sigismember (&old_sigset, i);

		/* there is no signal with this value: there are gaps in
		 * the list.
		 */
		if (sigaction (i, NULL, &act) < 0)
			continue;

		if (act.sa_handler == SIG_IGN)
			ignored = 1;

		if (rc < 0)
			continue;
		else if (rc)
			blocked = 1;

		show ("%s ('%s', %d): blocked=%s, ignored=%s",
				get_signal_name (i),
				strsignal (i),
				i,
				blocked ? YES_STR : NO_STR,
				ignored ? YES_STR : NO_STR);
	}
}


void
show_rlimits (void)
{
	header ("limits");

	show_limit (RLIMIT_AS);
	show_limit (RLIMIT_CORE);
	show_limit (RLIMIT_CPU);
	show_limit (RLIMIT_DATA);
	show_limit (RLIMIT_FSIZE);

	/* why can't we use this? */
#if 0
	show_limit (RLIMIT_RTTIME);
#endif

#ifdef linux
	show_limit (RLIMIT_LOCKS);
#endif

	show_limit (RLIMIT_MEMLOCK);

#ifdef linux
	show_limit (RLIMIT_MSGQUEUE);
	show_limit (RLIMIT_NICE);
#endif

	show_limit (RLIMIT_NOFILE);
	show_limit (RLIMIT_NPROC);
	show_limit (RLIMIT_RSS);

#ifdef linux
	show_limit (RLIMIT_RTPRIO);
#endif

	/* FIXME */
#if 0
	show_limit (RLIMIT_RTTIME);
#endif

#ifdef linux
	show_limit (RLIMIT_SIGPENDING);
#endif

	show_limit (RLIMIT_STACK);
}

void
dump_sysconf (void)
{
	struct procenv_map *p;
	long                value;

	header ("sysconf");

	for (p = sysconf_map; p && p->name; p++) {
		value = get_sysconf (p->num);
		show ("%s=%ld", p->name, value);
	}
}

void
show_confstrs (void)
{
	header ("confstr");

#ifdef linux
	show_confstr (_CS_GNU_LIBC_VERSION);
	show_confstr (_CS_GNU_LIBPTHREAD_VERSION);
#endif
	show_confstr (_CS_PATH);
}

void
get_misc (void)
{
	misc.umask_value = umask (S_IWGRP|S_IWOTH);
	assert (getcwd (misc.cwd, sizeof (misc.cwd)));

#if defined (PROCENV_LINUX)
	get_root (misc.root, sizeof (misc.root));
#endif
#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	get_bsd_misc ();
#endif
}

/**
 * is_console:
 * @fd: open file descriptor.
 *
 * Check if specified file descriptor is attached to a _console_
 * device (physical or virtual).
 *
 * Note that ptys are NOT consoles :)
 *
 * Returns: TRUE if @fd is attached to a console, else FALSE.
 *
 * FIXME: how can this be determined for BSD?
 **/
int
is_console (int fd)
{
#if defined (PROCENV_LINUX)
	struct vt_mode vt;
	int ret;

	ret = ioctl (0, VT_GETMODE, &vt);

	return !ret;
#else
	return FALSE;
#endif
}

void
dump_user (void)
{

	header ("process");

	show ("process id (pid): %d", user.pid);

	show ("parent process id (ppid): %d", user.ppid);
	show ("session id (sid): %d (leader=%s)",
			user.sid,
			is_session_leader () ? YES_STR : NO_STR);

	show ("name: '%s'", user.proc_name);

	show_proc_branch ();

	show ("process group id: %d (leader=%s)",
			user.pgroup,
			is_process_group_leader () ? YES_STR : NO_STR);
	
	show ("foreground process group: %d", user.fg_pgroup);

	show ("terminal: '%s'", user.ctrl_terminal);

	show ("has controlling terminal: %s",
			has_ctty () ? YES_STR : NO_STR);

#if defined (PROCENV_LINUX)
	show ("on console: %s",
			is_console (user.tty_fd) ? YES_STR : NO_STR);
#else
	show ("on console: %s", UNKNOWN_STR);
#endif

	show ("real user id (uid): %d ('%s')",
			user.uid,
			get_user_name (user.uid));

	show ("effective user id (euid): %d ('%s')",
			user.euid,
			get_user_name (user.euid));

	show ("saved set-user-id (suid): %d ('%s')",
			user.suid,
			get_user_name (user.suid));

	show ("real group id (gid): %d ('%s')",
			user.gid,
			get_group_name (user.gid));

	show ("effective group id (egid): %d ('%s')",
			user.egid,
			get_group_name (user.egid));

	show ("saved set-group-id (sgid): %d ('%s')",
			user.sgid,
			get_group_name (user.sgid));

	show ("login name: '%s'", user.login ? user.login : "");

	show ("passwd: name='%s', gecos='%s', dir='%s', shell='%s'",
			user.passwd.pw_name,
			user.passwd.pw_gecos,
			user.passwd.pw_dir,
			user.passwd.pw_shell);
	show_all_groups ();
}

void
dump_priorities (void)
{
	show ("scheduler priority: process=%d, process group=%d, user=%d",
			priority.process,
			priority.pgrp,
			priority.user);
}

void
dump_misc (void)
{
	header ("misc");

	show ("umask: %4.4o", misc.umask_value);
	show ("current directory (cwd): '%s'", misc.cwd);
#if defined (PROCENV_LINUX)
	show ("root: '%s'", misc.root);
#endif
	show ("chroot: %s", in_chroot () ? YES_STR : NO_STR);
#if defined (PROCENV_LINUX)
	show_linux_selinux_context ();
#endif
	show ("container: %s", container_type ());

	show_cpu ();
	dump_priorities ();
	show ("memory page size: %d bytes", getpagesize ());
}

void
show_cpu (void)
{
#if defined (PROCENV_LINUX)
	show_linux_cpu ();
	show_linux_scheduler ();
#endif

#if defined (PROCENV_BSD)
	show_bsd_cpu ();
#endif
}

void
dump_fds (void)
{
	int fd;
	int max;

	header ("fds");
	max = sysconf (_SC_OPEN_MAX);

	for (fd = 0; fd < max; fd++) {
		if (fd_valid (fd)) {
			int is_tty = isatty (fd);
			char *name = NULL;

			if (is_tty) {
				name = ttyname (fd);
				show ("fd %d: terminal=%s ('%s')", fd,
						is_tty ? YES_STR : NO_STR,
						name ? name : NA_STR);
			} else {
				show ("fd %d: terminal=%s", fd, NO_STR);
			}
		}
	}
#if defined (PROCENV_LINUX)
	dump_linux_proc_fds ();
#endif

}

void
show_env (void)
{
	char **env = environ;

	header ("environment");

	while (env && *env) {
		show ("%s", *env);
		env++;
	}
}

int
strcmp_compar (const void *a, const void *b)
{
	return strcmp (*(char * const *)a, *(char * const *)b);
}

void
get_user_info (void)
{
	struct passwd *pw;
	struct group  *gr;
	void          *p;
	int            fd;
	int            ret;

	user.pid  = getpid ();
	user.ppid = getppid ();

#if defined (PROCENV_LINUX)
	if (prctl (PR_GET_NAME, user.proc_name, 0, 0, 0) < 0)
		die ("unable to query process details");
#endif

#ifdef _GNU_SOURCE
	ret = getresuid (&user.uid, &user.euid, &user.suid);
	assert (! ret);

	getresgid (&user.gid, &user.egid, &user.sgid);
	assert (! ret);
#else
	/* NB: no saved uid+gid */
	user.uid  = getuid ();
	user.euid = geteuid ();
	user.gid  = getgid ();
	user.egid = getegid ();
#endif

	user.sid = getsid ((pid_t)0);

	errno = 0;

	/*
	 * XXX: This will be NULL if auditd isn't running (for example
	 * on Ubuntu Desktop systems).
	 *
	 * See Question 8 here:
	 *
	 * http://people.redhat.com/sgrubb/audit/audit-faq.txt
	 */
	user.login = getlogin ();
	user.pgroup = getpgrp ();

	ctermid (user.ctrl_terminal);

	/* Get a reference to the controlling terminal
	 * in case all standard fds are redirected.
	 *
	 * If run from a process superviser such as Upstart, setsid()
	 * will already have been called which means it is impossible to
	 * regain a controlling terminal. Thus, if the open fails,
	 * attempt to use STDIN_FILENO (however, this will probably fail
	 * too as it will be redirected to /dev/null.
	 */

	/* open r/w for OUTPUT_TERM's benefit */
	user.tty_fd = open (user.ctrl_terminal, O_RDWR);
	if (user.tty_fd < 0)
		user.tty_fd = STDIN_FILENO;

	user.fg_pgroup = tcgetpgrp (user.tty_fd);
	user.pgid_sid = tcgetsid (user.tty_fd);

	errno = 0;
	pw = getpwuid (user.uid);
	if (!pw && errno == 0)
		die ("uid %d no longer exists", user.uid);

	p = memcpy (&user.passwd, pw, sizeof (struct passwd));
	assert (p == (void *)&user.passwd);
}

/* append @new to @orig */
void
append (char **str, const char *new)
{
    //char    **p;
    size_t    len;
    size_t    total;

    assert (str);
    assert (new);

    /* +1 for terminating nul */
    total = strlen (*str) + 1;

    len = strlen (new);

    /* +1 for comma-delimiter */
    total += len + 1;

    *str = realloc (*str, total);
    assert (*str);
    strcat (*str, new);
}

/* append fmt and args to @str */
void
appendf (char **str, const char *fmt, ...)
{
    va_list  ap;
    char    *new = NULL;

    assert (str);
    assert (fmt);

    va_start (ap, fmt);

    if (vasprintf (&new, fmt, ap) < 0) {
        perror ("vasprintf");
        exit (EXIT_FAILURE);
    }

    va_end (ap);

    append (str, new);
    free (new);
}

void
appendva (char **str, const char *fmt, va_list ap)
{
    char  *new = NULL;

    assert (str);
    assert (fmt);

    if (vasprintf (&new, fmt, ap) < 0) {
        perror ("vasprintf");
        exit (EXIT_FAILURE);
    }

    append (str, new);
    free (new);
}

void
show_all_groups (void)
{
	int     i;
	int     ret;
	char   *str;

	/* Initial number of groups we'll try to read. If this isn't
	 * enough, we increase it to make rooom for all available
	 * groups. So don't worry :)
	 */
	int     size = 32;

	gid_t  *groups = NULL;
	char  **group_names = NULL;

	groups = malloc (size * sizeof (gid_t));
	if (! groups)
		goto error;

	while (TRUE) {
		ret = getgroups (size, groups);
		if (ret >= 0)
			break;

		size++;
		groups = realloc (groups, (size * sizeof (gid_t)));
		if (! groups)
			goto error;
	}

	str = strdup ("groups:");
	assert (str);

	size = ret;

	if (size == 0) {
		char *group;

		free (groups);

		group = get_group_name (user.passwd.pw_gid);
		if (! group) {
			show (str);
			free (str);
			return;
		}

		appendf (&str, " '%s' (%d)",
				group,
				user.passwd.pw_gid);
		show (str);
		free (str);
		return;
	}

	group_names = calloc (size, sizeof (char *));
	if (! group_names)
		die ("failed to allocate space for group array");

	/* spacer */
	appendf (&str, " ");

	for (i = 0; i < size; i++) {
		ret = asprintf (&group_names[i], "'%s' (%d)",
				get_group_name (groups[i]), groups[i]);
		if (ret < 0)
			die ("unable to create group entry");
	}

	qsort (group_names, size, sizeof (char *), strcmp_compar);

	for (i = 0; i < size; i++) {
		if (i+1 == size)
			appendf (&str, "%s", group_names[i]);
		else
			appendf (&str, "%s, ", group_names[i]);
		free (group_names[i]);
	}
	free (group_names);
	free (groups);

	show (str);
	free (str);

	return;

error:
	die ("failed to allocate space for groups");
}

void
set_indent (void)
{
	indent = selected_option ? 0 : INDENT;
}

void
init (void)
{
	set_indent ();
	get_user_info ();
	get_misc ();
	get_priorities ();
}

void
cleanup (void)
{
	close (user.tty_fd);

	if (output_fd != -1)
		close (output_fd);

	if (output == OUTPUT_SYSLOG)
		closelog ();
}

void
dump_meta (void)
{
	long bits;
	header (PACKAGE_NAME);

	show ("version: %s", PACKAGE_STRING);
	show ("platform: %s", get_platform ());
	bits = get_kernel_bits ();

	if (bits == -1)
		show ("kernel bits: %s", UNKNOWN_STR);
	else
		show ("kernel bits: %ld", bits);
}

void
show_stat (void)
{
	struct stat  st;
	char         real_path[PATH_MAX];
	char         formatted_time[32];
	char         modestr[10+1];
	mode_t       perms;
	int          i = 0;
	char        *tmp = NULL;

	assert (program_name);
	assert (misc.cwd);

	tmp = strdup ("");
	assert (tmp);

	tmp = get_path (program_name);
	assert (tmp);

	if (! realpath (tmp, real_path))
		die ("unable to resolve path");

	free (tmp);

	if (stat (real_path, &st) < 0)
		die ("unable to stat path: '%s'", real_path);

	header ("stat");
	show ("argv[0]: '%s'", program_name);
	show ("real path: '%s'", real_path);
	show ("dev: major=%u, minor=%u",
			major (st.st_dev), minor (st.st_dev));
	show ("inode: %lu", (unsigned long int)st.st_ino);

	memset (modestr, '\0', sizeof (modestr));

	modestr[i++] = (S_ISLNK (st.st_mode & S_IFMT)) ? 'l' : '-';

	perms = (st.st_mode & S_IRWXU);
	modestr[i++] = (perms & S_IRUSR) ? 'r' : '-';
	modestr[i++] = (perms & S_IWUSR) ? 'w' : '-';
	modestr[i++] = (perms & S_IXUSR) ? 'x' : '-';

	perms = (st.st_mode & S_IRWXG);
	modestr[i++] = (perms & S_IRGRP) ? 'r' : '-';
	modestr[i++] = (perms & S_IWGRP) ? 'w' : '-';
	modestr[i++] = (perms & S_IXGRP) ? 'x' : '-';

	perms = (st.st_mode & S_IRWXO);
	modestr[i++] = (perms & S_IROTH) ? 'r' : '-';
	modestr[i++] = (perms & S_IWOTH) ? 'w' : '-';
	modestr[i++] = (perms & S_IXOTH) ? 'x' : '-';

	perms = (st.st_mode &= ~S_IFMT);
	if (perms & S_ISUID)
		modestr[3] = 's';
	if (perms & S_ISGID)
		modestr[6] = 's';
	if (perms & S_ISVTX)
		modestr[9] = 't';

	show ("mode: %4.4o (%s)", perms, modestr);

	show ("hard links: %u", st.st_nlink);
	show ("user id (uid): %d ('%s')", st.st_uid, get_user_name (st.st_uid));
	show ("group id (gid): %d ('%s')", st.st_gid, get_group_name (st.st_uid));
	show ("size: %lu bytes (%lu 512-byte blocks)", st.st_size, st.st_blocks);

	if (! ctime_r (&st.st_atime, formatted_time))
		die ("failed to format atime");
	formatted_time[ strlen (formatted_time)-1] = '\0';
	show ("atime: %lu (%s)", st.st_atime, formatted_time);


	if (! ctime_r (&st.st_mtime, formatted_time))
		die ("failed to format mtime");
	formatted_time[ strlen (formatted_time)-1] = '\0';
	show ("mtime: %lu (%s)", st.st_mtime, formatted_time);

	if (! ctime_r (&st.st_ctime, formatted_time))
		die ("failed to format ctime");
	formatted_time[ strlen (formatted_time)-1] = '\0';
	show ("ctime: %lu (%s)", st.st_ctime, formatted_time);
}

long
get_kernel_bits (void)
{
#if defined (PROCENV_LINUX)
	long value;

	errno = 0;
	value = sysconf (_SC_LONG_BIT);
	if (value == -1 && errno != 0)
		die ("failed to determine kernel bits");
	return value;
#endif
	return -1;
}

void
dump (void)
{
	dump_meta ();

#if defined (PROCENV_LINUX)
	show_capabilities ();
	show_linux_cgroups ();
#endif
	show_clocks ();
	show_compiler ();
	show_confstrs ();
	show_env ();
	dump_fds ();
	show_libs ();
	show_rlimits ();
	dump_misc ();
	show_mounts (SHOW_ALL);
	show_oom ();
	dump_user ();
	show_ranges ();
	show_signals ();
	show_sizeof ();
	show_stat ();
	dump_sysconf ();
	show_time ();
	show_timezone ();
	show_tty_attrs ();
	show_uname ();
}

#if defined (PROCENV_LINUX)
void
show_linux_mounts (ShowMountType what)
{
	FILE *mtab;
	struct mntent *mnt;
	int major, minor;

	mtab = fopen (MOUNTS, "r");

	if (! mtab)
		die ("unable to read mounts");

	while ((mnt = getmntent (mtab))) {
		if (what == SHOW_ALL || what == SHOW_MOUNTS) {
			get_major_minor (mnt->mnt_dir,
					&major,
					&minor);
			show ("fsname='%s', dir='%s', type='%s', "
					"opts='%s', "
					"dev=(major:%d, minor:%d)",
					mnt->mnt_fsname,
					mnt->mnt_dir,
					mnt->mnt_type,
					mnt->mnt_opts,
					major,
					minor);
		}

		if (what == SHOW_ALL || what == SHOW_PATHCONF)
			show_pathconfs (what, mnt->mnt_dir);
	}

	fclose (mtab);
}
#endif

void
show_mounts (ShowMountType what)
{
	header ("mounts");

#if defined (PROCENV_LINUX)
	show_linux_mounts (what);
#endif

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	show_bsd_mounts (what);
#endif
}

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)

char *
get_bsd_mount_opts (uint64_t flags)
{
	struct mntopt_map  *opt;
	char               *str = NULL;
	int                 first = TRUE;
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

void
show_bsd_mounts (ShowMountType what)
{
	int             count;
	struct statfs  *mounts;
	struct statfs  *mnt;
	int             i;

	/* Note that returned memory cannot be freed (by us) */
	count = getmntinfo (&mounts, MNT_WAIT);

	if (! count)
		die ("unable to query mount info");

	mnt = mounts;
	
	for (i = 0; i < count; i++) {
		char *opts = NULL;

		opts = get_bsd_mount_opts (mnt->f_flags);
		if (! opts)
			die ("cannot determine FS flags for mountpoint '%s'",
					mnt->f_mntonname);

		if (what == SHOW_ALL || what == SHOW_MOUNTS) {
			show ("fsname='%s', dir='%s', type='%s', opts='%s'",
					mnt->f_mntfromname,
					mnt->f_mntonname,
					mnt->f_fstypename,
					opts);
		}
		if (what == SHOW_ALL || what == SHOW_PATHCONF)
			show_pathconfs (what, mnt->f_mntonname);
		mnt++;

		free (opts);
	}
}
#endif

void
get_priorities (void)
{
	priority.process = getpriority (PRIO_PROCESS, 0);
	assert (priority.process != (pid_t)-1);

	priority.pgrp = getpriority (PRIO_PGRP, 0);
	assert (priority.pgrp != (pid_t)-1);

	priority.user = getpriority (PRIO_USER, 0);
	assert (priority.user != (pid_t)-1);
}

bool
uid_match (uid_t uid)
{
	return uid == getuid ();
}


/**
 * in_container:
 *
 * Determine if running inside a container.
 *
 * Returns: Name of container type, or NO_STR.
 **/
const char *
container_type (void)
{
	struct stat  statbuf;
	dev_t        expected;
	char         buffer[1024];
	FILE        *f;

	expected = makedev (5, 1);

	if (stat ("/dev/console", &statbuf) < 0)
		goto out;

#if defined (PROCENV_BSD)
	if (misc.in_jail)
		return "jail";
#endif
	/* LXC's /dev/console is actually a pty */
#if defined (PROCENV_LINUX)
	if (major (statbuf.st_rdev) != major (expected)
			|| (minor (statbuf.st_rdev)) != minor (expected))
		return "lxc";
#endif

	if (! stat ("/proc/vz", &statbuf) && stat ("/proc/bc", &statbuf) < 0)
		return "openvz";

	f = fopen ("/proc/self/status", "r");
	if (! f)
		goto out;

	while (fgets (buffer, sizeof (buffer), f)) {
		size_t len = strlen (buffer);
		buffer[len-1] = '\0';

		if (strstr (buffer, "VxID") == buffer) {
			fclose (f);
			return "vserver";
		}
	}

	fclose (f);

out:
	return NO_STR;
}

/**
 * in_chroot:
 *
 * Determine if running inside a chroot environment.
 *
 * Failures are fatal.
 *
 * Returns TRUE if within a chroot, else FALSE.
 **/
bool
in_chroot (void)
{
	struct stat st;
	int i;
	int root_inode, self_inode;
	char root[] = "/";
	char self[] = "/proc/self/root";
	char bsd_self[] = "/proc/curproc";
	char *dir = NULL;

	i = stat (root, &st);
	if (i != 0) {
		dir = root;
		goto error;
	}

	root_inode = st.st_ino;

	/*
	 * Inode 2 is the root inode for most filesystems. However, XFS
	 * uses 128 for root.
	 */
	if (root_inode != 2 && root_inode != 128)
		return TRUE;

	i = stat (bsd_self, &st);
	if (i == 0) {
		/* Give up here if running on BSD */
		return FALSE;
	}

	i = stat (self, &st);
	if (i != 0)
		return FALSE;

	self_inode = st.st_ino;

	if (root_inode == self_inode)
		return FALSE;

	return TRUE;

error:
	die ("cannot stat '%s'", dir);
}

/* detect if setsid(2) has been called */
bool
is_session_leader (void)
{
	return user.sid == user.pid;
}

/* detect if setpgrp(2)/setpgid(2) (or setsid(2)) has been called */
bool
is_process_group_leader (void)
{
	return user.pgroup == user.pid;
}

void
show_proc_branch (void)
{
#if defined (PROCENV_LINUX)
	show_linux_proc_branch ();
#endif

#if defined (PROCENV_BSD)
	show_bsd_proc_branch ();
#endif
}

#if defined (PROCENV_BSD)
void
show_bsd_proc_branch (void)
{
	int                  count = 0;
	int                  i;
	char                 errors[_POSIX2_LINE_MAX];
	kvm_t               *kvm;
	struct kinfo_proc   *procs;
	struct kinfo_proc   *p;
	pid_t                self, current;
	int                  done = FALSE;
	char                *str;

	str = strdup ("ancestry: ");
	assert (str);

	self = current = getpid ();

	kvm = kvm_openfiles (NULL, _PATH_DEVNULL, NULL, O_RDONLY, errors);
	if (! kvm)
		die ("unable to open kvm");

	procs = kvm_getprocs (kvm, KERN_PROC_PROC, 0, &count);
	if (! procs)
		die ("failed to get process info");

	while (! done) {
		for (i = 0; i < count; i++) {
			p = &procs[i];

			if (p->ki_pid == current) {
				if (current == 0) {
					/* ultimate parent == PID 0 == '[kernel]' */
					appendf (&str, "%d ('%s')",
							(int)current, p->ki_comm);
					done = TRUE;
					break;
				} else {
					appendf (&str, "%d ('%s'), ",
							(int)current, p->ki_comm);
				}
				current = p->ki_ppid;
			}
		}
	}

	if (kvm_close (kvm) < 0)
		die ("failed to close kvm");

	show (str);
	free (str);
}
#endif

#if defined (PROCENV_LINUX)
void
show_linux_proc_branch (void)
{
	char    buffer[1024];
	char    path[PATH_MAX];
	char    name[16];
	char    pid[16];
	char    ppid[16];
	size_t  len;
	char   *p;
	FILE   *f;
	char   *str;

	str = strdup ("ancestry: ");
	assert (str);

	sprintf (pid, "%d", (int)getpid ());

	/* This is one God-awful interface */
	while (TRUE) {
		sprintf (path, "/proc/%s/status", pid);

		f = fopen (path, "r");
		if (! f)
			return;

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

	show (str);
	free (str);
}
#endif

/* More verbose version of stty(1).
 *
 * Tries to obtain a tty fd by considering the standard streams if the
 * default fails.
 **/
void
show_tty_attrs (void)
{
	struct termios tty;
	int            ret;
	int            fds[4] = { -1, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO };

	fds[0] = user.tty_fd;

	int i;

	header ("tty");

	for (i = 0; i < sizeof (fds) / sizeof (fds[0]); i++) {
		if (! isatty (fds[i]))
			continue;
		else {
			ret = tcgetattr (fds[i], &tty);
			if (! ret)
				goto work;
		}
	}

out_of_ideas:
	show ("%s", NA_STR);
	return;

work:

	user.tty_fd = fds[i];

	show ("c_iflag=0x%x", tty.c_iflag);

	show_const (tty, c_iflag, IGNBRK);
	show_const (tty, c_iflag, BRKINT);
	show_const (tty, c_iflag, IGNPAR);
	show_const (tty, c_iflag, PARMRK);
	show_const (tty, c_iflag, INPCK);
	show_const (tty, c_iflag, ISTRIP);
	show_const (tty, c_iflag, INLCR);
	show_const (tty, c_iflag, IGNCR);
	show_const (tty, c_iflag, ICRNL);
#if defined (PROCENV_LINUX)
	show_const (tty, c_iflag, IUCLC);
#endif
	show_const (tty, c_iflag, IXON);
	show_const (tty, c_iflag, IXANY);
	show_const (tty, c_iflag, IXOFF);
	show_const (tty, c_iflag, IMAXBEL);
#if defined (PROCENV_LINUX)
	show_const (tty, c_iflag, IUTF8);
#endif

	show ("c_oflag=0x%x", tty.c_oflag);

	show_const (tty, c_oflag, OPOST);
#if defined (PROCENV_LINUX)
	show_const (tty, c_oflag, OLCUC);
#endif
	show_const (tty, c_oflag, ONLCR);
	show_const (tty, c_oflag, OCRNL);
	show_const (tty, c_oflag, ONOCR);
	show_const (tty, c_oflag, ONLRET);
#if defined (PROCENV_LINUX)
	show_const (tty, c_oflag, OFILL);
	show_const (tty, c_oflag, OFDEL);
	show_const (tty, c_oflag, NLDLY);
	show_const (tty, c_oflag, CRDLY);
#endif
	show_const (tty, c_oflag, TABDLY);
#if defined (PROCENV_LINUX)
	show_const (tty, c_oflag, BSDLY);
	show_const (tty, c_oflag, VTDLY);
	show_const (tty, c_oflag, FFDLY);
#endif

	show ("c_cflag=0x%x", tty.c_cflag);

	show ("  [c_cflag:input baud speed=%s]",
			get_speed (cfgetispeed (&tty)));

	show ("  [c_cflag:output baud speed=%s]",
			get_speed (cfgetospeed (&tty)));

#if defined (PROCENV_LINUX)
	show_const (tty, c_cflag, CBAUDEX);
#endif
	show_const (tty, c_cflag, CSIZE);
	show_const (tty, c_cflag, CSTOPB);
	show_const (tty, c_cflag, CREAD);
	show_const (tty, c_cflag, PARENB);
	show_const (tty, c_cflag, PARODD);
	show_const (tty, c_cflag, HUPCL);
	show_const (tty, c_cflag, CLOCAL);
#if defined (PROCENV_LINUX)
	show_const (tty, c_cflag, CIBAUD);
	show_const (tty, c_cflag, CMSPAR);
#endif
	show_const (tty, c_cflag, CRTSCTS);

	show ("c_lflag=0x%x", tty.c_lflag);

	show_const (tty, c_lflag, ISIG);
#if defined (PROCENV_LINUX)
	show_const (tty, c_lflag, XCASE);
#endif
	show_const (tty, c_lflag, ICANON);
	show_const (tty, c_lflag, ECHO);
	show_const (tty, c_lflag, ECHOE);
	show_const (tty, c_lflag, ECHOK);
	show_const (tty, c_lflag, ECHONL);
	show_const (tty, c_lflag, ECHOCTL);
	show_const (tty, c_lflag, ECHOPRT);
	show_const (tty, c_lflag, ECHOKE);
	show_const (tty, c_lflag, FLUSHO);
	show_const (tty, c_lflag, NOFLSH);
	show_const (tty, c_lflag, TOSTOP);
	show_const (tty, c_lflag, PENDIN);
	show_const (tty, c_lflag, IEXTEN);

	show ("c_cc:");

	show_cc (tty, VINTR);
	show_cc (tty, VQUIT);
	show_cc (tty, VERASE);
	show_cc (tty, VKILL);
	show_cc (tty, VEOF);
	show_cc (tty, VTIME);
	show_cc (tty, VMIN);
#if defined (PROCENV_LINUX)
	show_cc (tty, VSWTC);
#endif
	show_cc (tty, VSTART);
	show_cc (tty, VSTOP);
	show_cc (tty, VSUSP);
	show_cc (tty, VEOL);
	show_cc (tty, VREPRINT);
	show_cc (tty, VDISCARD);
	show_cc (tty, VWERASE);
	show_cc (tty, VLNEXT);
	show_cc (tty, VEOL2);

}


const char *
get_signal_name (int signum)
{
	assert (signum);

	struct procenv_map *p;

	for (p = signal_map; p && p->name; p++) {
		if (signum == p->num)
			return p->name;
	}

	return NULL;
}

char *
get_platform (void)
{

#ifdef _AIX
	return "AIX";
#endif

#ifdef __FreeBSD__
	return "FreeBSD";
#endif
#ifdef _OpenBSD__
	return "OpenBSD";
#endif
#ifdef _NetBSD__
	return "NetBSD";
#endif

#ifdef __hpux
	return "HP-UX";
#endif

#ifdef OS400
	return "iSeries";
#endif

#ifdef __FreeBSD_kernel__
	return "kFreeBSD";
#endif

#ifdef linux
#ifdef __s390__
	return "Linux (zSeries)";
#endif
#if defined (__x86_64) || defined (__amd64)
	return "Linux (x64/AMD64)";
#endif
#ifdef __i386__
	return "Linux (Intel)";
#endif
#ifdef __powerpc__
	return "Linux (PPC)";
#endif
#endif

#ifdef VMS
	return "OpenVMS";
#endif

#ifdef sun
	return "Solaris";
#endif

#ifdef __osf__
	return "Tru64";
#endif

#ifdef WINDOWS
	return "Windows";
#endif

	return "<unknown>";
}

int
libs_callback (struct dl_phdr_info *info, size_t size, void *data)
{
	if (info->dlpi_name && *info->dlpi_name)
		show ("%s", info->dlpi_name);

	return 0;
}

void
show_libs (void)
{
	header ("libs");

	dl_iterate_phdr (libs_callback, NULL);
}

void
show_clocks (void)
{
	header ("clocks");

	show_clock_res (CLOCK_REALTIME);

#if defined (__FreeBSD__)
	show_clock_res (CLOCK_REALTIME_PRECISE);
	show_clock_res (CLOCK_REALTIME_FAST);
#endif

	show_clock_res (CLOCK_MONOTONIC);

#if defined (__FreeBSD__) || defined (__FreeBSD_kernel__)
	show_clock_res (CLOCK_MONOTONIC_PRECISE);
	show_clock_res (CLOCK_MONOTONIC_FAST);
	show_clock_res (CLOCK_UPTIME);
	show_clock_res (CLOCK_UPTIME_PRECISE);
	show_clock_res (CLOCK_UPTIME_FAST);
	show_clock_res (CLOCK_VIRTUAL);
#endif

#if defined (__FreeBSD__)
	show_clock_res (CLOCK_PROF);
#endif

#if defined (PROCENV_LINUX)
#ifdef CLOCK_MONOTONIC_RAW
	show_clock_res (CLOCK_MONOTONIC_RAW);
#endif
	show_clock_res (CLOCK_PROCESS_CPUTIME_ID);
	show_clock_res (CLOCK_THREAD_CPUTIME_ID);
#endif
}

void
show_timezone (void)
{
#if defined (PROCENV_LINUX)
	tzset ();

	header ("timezone");

	show ("tzname[0]='%s'", tzname[0]);
	show ("tzname[1]='%s'", tzname[1]);
	show ("timezone=%ld", timezone);
	show ("daylight=%d", daylight);
#endif
}

#define show_size(thing) \
	show ("sizeof (" #thing "): %lu byte%s", \
			(unsigned long int)sizeof (thing), \
			sizeof (thing) == 1 ? "" : "s")

void
show_sizeof (void)
{
	header ("sizeof");

	show ("bits/byte (CHAR_BIT): %d", CHAR_BIT);

	/* fundamental types and non-aggregate typedefs */

	show_size (char);
	show_size (short int);
	show_size (int);

	show_size (long int);

	show_size (long long int);

	show_size (float);

	show_size (double);

	show_size (long double);

	show_size (size_t);
	show_size (ssize_t);
	show_size (ptrdiff_t);
	show_size (void *);
	show_size (wchar_t);

	show_size (intmax_t);
	show_size (intptr_t);
	show_size (uintptr_t);

	show_size (time_t);
	show_size (clock_t);

	show_size (sig_atomic_t);
	show_size (off_t);
	show_size (fpos_t);
	show_size (mode_t);

	show_size (pid_t);
	show_size (uid_t);
	show_size (gid_t);

	show_size (wint_t);
	show_size (div_t);
	show_size (ldiv_t);
	show_size (lldiv_t);
	show_size (mbstate_t);
}

void
show_ranges (void)
{
	header ("ranges");

	/******************************/
	show ("char:");

	showi (INDENT, "unsigned: %u to %u (%e to %e, 0x%.*x to 0x%.*x)",
			0, UCHAR_MAX,
			(double)0, (double)UCHAR_MAX,
			type_hex_width (char), 0,
			type_hex_width (char), UCHAR_MAX);
	showi (INDENT, "signed: %d to %d", CHAR_MIN, CHAR_MAX);

	/******************************/
	show ("short int:");

	showi (INDENT, "unsigned: %u to %u (%e to %e, 0x%.*x to 0x%.*x)",
			0, USHRT_MAX,
			(double)0, (double)USHRT_MAX,
			type_hex_width (short int), 0,
			type_hex_width (short int), USHRT_MAX);

	showi (INDENT, "signed: %d to %d", SHRT_MIN, SHRT_MAX);

	/******************************/
	show ("int:");

	showi (INDENT, "unsigned: %u to %u (%e to %e, 0x%.*x to 0x%.*x)",
			0, UINT_MAX,
			(double)0, (double)UINT_MAX,
			type_hex_width (int), 0,
			type_hex_width (int), UINT_MAX);
	showi (INDENT, "signed: %d to %d", INT_MIN, INT_MAX);

	/******************************/
	show ("long int:");

	showi (INDENT, "unsigned: %lu to %lu (%e to %e, 0x%.*lx to 0x%.*lx)",
			0L, ULONG_MAX,
			(double)0, (double)ULONG_MAX,
			type_hex_width (long int), 0L,
			type_hex_width (long int), ULONG_MAX);
	showi (INDENT, "signed: %ld to %ld", LONG_MIN, LONG_MAX);

	/******************************/
	show ("long long int:");

	showi (INDENT, "unsigned: %llu to %llu (%e to %e, 0x%.*llx to 0x%.*llx)",
			0LL, ULLONG_MAX,
			(double)0LL, (double)ULLONG_MAX,
			type_hex_width (long long int), 0LL,
			type_hex_width (long long int), ULLONG_MAX);
	showi (INDENT, "signed: %lld to %lld", LLONG_MIN, LLONG_MAX);
	/******************************/
	show ("float:");

	showi (INDENT, "signed: %e to %e", FLT_MIN, FLT_MAX);

	/******************************/
	show ("double:");

	showi (INDENT, "signed: %le to %le", DBL_MIN, DBL_MAX);

	/******************************/
	show ("long double:");

	showi (INDENT, "signed: %Le to %Le", LDBL_MIN, LDBL_MAX);

	/******************************/
}

void
show_compiler (void)
{
	char *name;
	char *version;

#if defined (__GNUC__)
	name = "GCC";
	version = __VERSION__;
#elif defined (__clang__)
	name = "LLVM";
	version = __clang_version__;
#elif defined (__INTEL_COMPILER)
	name = "Intel";
	version = __ICC;
#endif

	header ("compiler");
	show ("name: %s", name);
	show ("version: %s", version);
	show ("compile date: %s", __DATE__);
	show ("compile time: %s", __TIME__);

#ifdef __STRICT_ANSI__
	show ("__STRICT_ANSI__: %s", DEFINED_STR);
#endif
	show ("__STRICT_ANSI__: %s", NOT_DEFINED_STR);

#ifdef _POSIX_C_SOURCE
	show ("_POSIX_C_SOURCE: %lu", _POSIX_C_SOURCE);
#else
	show ("_POSIX_C_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _POSIX_SOURCE
	show ("_POSIX_SOURCE: %s", DEFINED_STR);
#else
	show ("_POSIX_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _XOPEN_SOURCE
	show ("_XOPEN_SOURCE: %lu", _XOPEN_SOURCE);
#else
	show ("_XOPEN_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _XOPEN_SOURCE_EXTENDED
	show ("_XOPEN_SOURCE_EXTENDED: %s", DEFINED_STR);
#else
	show ("_XOPEN_SOURCE_EXTENDED: %s", NOT_DEFINED_STR);
#endif

#ifdef _ISOC95_SOURCE
	show ("_ISOC95_SOURCE: %s", DEFINED_STR);
#else
	show ("_ISOC95_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _ISOC99_SOURCE
	show ("_ISOC99_SOURCE: %s", DEFINED_STR);
#else
	show ("_ISOC99_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _ISOC11_SOURCE
	show ("_ISOC11_SOURCE: %s", DEFINED_STR);
#else
	show ("_ISOC11_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _LARGEFILE64_SOURCE
	show ("_LARGEFILE64_SOURCE: %s", DEFINED_STR);
#else
	show ("_LARGEFILE64_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _FILE_OFFSET_BITS
	show ("_FILE_OFFSET_BITS: %lu", _FILE_OFFSET_BITS);
#else
	show ("_FILE_OFFSET_BITS: %s", NOT_DEFINED_STR);
#endif

#ifdef _BSD_SOURCE
	show ("_BSD_SOURCE: %s", DEFINED_STR);
#else
	show ("_BSD_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _SVID_SOURCE
	show ("_SVID_SOURCE: %s", DEFINED_STR);
#else
	show ("_SVID_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _ATFILE_SOURCE
	show ("_ATFILE_SOURCE: %s", DEFINED_STR);
#else
	show ("_ATFILE_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _GNU_SOURCE
	show ("_GNU_SOURCE: %s", DEFINED_STR);
#else
	show ("_GNU_SOURCE: %s", NOT_DEFINED_STR);
#endif

#ifdef _REENTRANT
	show ("_REENTRANT: %s", DEFINED_STR);
#else
	show ("_REENTRANT: %s", NOT_DEFINED_STR);
#endif

#ifdef _THREAD_SAFE
	show ("_THREAD_SAFE: %s", DEFINED_STR);
#else
	show ("_THREAD_SAFE: %s", NOT_DEFINED_STR);
#endif

#ifdef _FORTIFY_SOURCE
	show ("_FORTIFY_SOURCE: %s", DEFINED_STR);
#else
	show ("_FORTIFY_SOURCE: %s", NOT_DEFINED_STR);
#endif


}

void
show_time (void)
{
	char              formatted_time[32];
	struct timespec   ts;
	struct tm        *tm;

	if (clock_gettime (CLOCK_REALTIME, &ts) < 0)
		die ("failed to determine time");

	tm = localtime (&ts.tv_sec);
	if (! tm)
		die ("failed to determine localtime");

	header ("time");
	show ("raw: %u.%lu",
			(unsigned int)ts.tv_sec,
			ts.tv_nsec);

	if (! asctime_r (tm, formatted_time))
		die ("failed to determine formatted time");

	/* overwrite trailing '\n' */
	formatted_time[ strlen (formatted_time)-1] = '\0';

	show ("local: %s", formatted_time);

	show ("ISO: %4.4d-%2.2d-%2.2dT%2.2d:%2.2d",
			1900+tm->tm_year,
			tm->tm_mon,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min);
}

void
show_uname (void)
{
	struct utsname uts;

	if (uname (&uts) < 0)
		die ("failed to query uname");

	header ("uname");

	show ("sysname: %s", uts.sysname);
	show ("nodename: %s", uts.nodename);
	show ("release: %s", uts.release);
	show ("version: %s", uts.version);
	show ("machine: %s", uts.machine);

#if defined (_GNU_SOURCE) && defined (linux)
	show ("domainname: %s", uts.domainname);
#endif
}

#if defined (PROCENV_LINUX)

#define show_capability(cap) \
{ \
	int ret; \
	\
	ret = prctl (PR_CAPBSET_READ, cap, 0, 0, 0); \
	\
	if (ret < 0) \
	die ("prctl failed for " #cap); \
	\
	show (#cap "=%s", ret ? YES_STR : NO_STR); \
}

void
show_capabilities (void)
{
	header ("capabilities(linux)");

	show_capability (CAP_CHOWN);
	show_capability (CAP_DAC_OVERRIDE);
	show_capability (CAP_DAC_READ_SEARCH);
	show_capability (CAP_FOWNER);
	show_capability (CAP_FSETID);
	show_capability (CAP_KILL);
	show_capability (CAP_SETGID);
	show_capability (CAP_SETUID);
	show_capability (CAP_SETPCAP);
	show_capability (CAP_LINUX_IMMUTABLE);
	show_capability (CAP_NET_BIND_SERVICE);
	show_capability (CAP_NET_BROADCAST);
	show_capability (CAP_NET_ADMIN);
	show_capability (CAP_NET_RAW);
	show_capability (CAP_IPC_LOCK);
	show_capability (CAP_IPC_OWNER);
	show_capability (CAP_SYS_MODULE);
	show_capability (CAP_SYS_RAWIO);
	show_capability (CAP_SYS_CHROOT);
	show_capability (CAP_SYS_PTRACE);
	show_capability (CAP_SYS_PACCT);
	show_capability (CAP_SYS_ADMIN);
	show_capability (CAP_SYS_BOOT);
	show_capability (CAP_SYS_NICE);
	show_capability (CAP_SYS_RESOURCE);
	show_capability (CAP_SYS_TIME);
	show_capability (CAP_SYS_TTY_CONFIG);
	show_capability (CAP_MKNOD);
	show_capability (CAP_LEASE);
	show_capability (CAP_AUDIT_WRITE);
	show_capability (CAP_AUDIT_CONTROL);
	show_capability (CAP_SETFCAP);
	show_capability (CAP_MAC_OVERRIDE);
	show_capability (CAP_MAC_ADMIN);
#ifdef CAP_SYSLOG
	show_capability (CAP_SYSLOG);
#endif
#ifdef CAP_WAKE_ALARM
	show_capability (CAP_WAKE_ALARM);
#endif
}

void
show_linux_selinux_context (void)
{
	char   *context;
	size_t  len;

#ifdef HAVE_SELINUX
	if (getpidcon (user.pid, &context) < 0)
		die ("failed to query SELinux context");

	len = strlen (context);

	/* don't show trailing NL */
	show ("selinux context: %.*s",
			len - 1,
			context);

	free (context);
#else
	show ("selinux context: %s", UNKNOWN_STR);
#endif

}

void
show_linux_cgroups (void)
{
	char *file = "/proc/self/cgroup";
	FILE *f;
	char buffer[1024];
	size_t len;

	f = fopen (file, "r");
	if (! f)
		return;

	header ("cgroup(linux)");

	while (fgets (buffer, sizeof (buffer), f)) {
		len = strlen (buffer);
		buffer[len-1] = '\0';
		show ("%s", buffer);
	}

	fclose (f);
}

void
dump_linux_proc_fds (void)
{
	DIR *dir;
	struct dirent *ent;
	struct stat st;
	char *prefix_path = "/proc/self/fd";
	char path[MAXPATHLEN];
	char link[MAXPATHLEN];
	int saved_errno;
	ssize_t len;

	header ("fds (linux/proc)");

	dir = opendir (prefix_path);
	if (!dir)
	{
		saved_errno = errno;
		show ("failed to open '%s'"
				" (errno=%d [%s])",
				prefix_path,
				saved_errno, strerror (saved_errno));
		exit (EXIT_FAILURE);
	}

	while ((ent=readdir (dir)) != NULL)
	{
		int fd;

		if ( !strcmp (ent->d_name, ".") || !strcmp (ent->d_name, "..") )
			continue;

		sprintf (path, "%s/%s", prefix_path, ent->d_name);
		fd = atoi (ent->d_name);

		len = readlink (path, link, sizeof (link));
		assert (len);
		link[len] = '\0';

		if (link[0] == '/') {

			if (stat (link, &st) < 0)
				continue;

			/* Ignore the last (invalid) entry */
			if (S_ISDIR (st.st_mode))
				continue;

		}

		show ("'%s' -> '%s' (terminal=%s, valid=%s)",
				path, link,
				isatty (fd) ? YES_STR : NO_STR,
				fd_valid (fd) ? YES_STR : NO_STR);
	}

	closedir (dir);
}

void
show_oom (void)
{
	char    *dir = "/proc/self";
	char    *files[] = { "oom_score", "oom_adj", "oom_score_adj", NULL };
	char    **file;
	FILE    *f;
	char     buffer[PROCENV_BUFFER];
	char     path[PATH_MAX];
	size_t   len;
	int      ret;

	header ("oom(linux)");

	for (file = files; file && *file; file++) {
		ret = sprintf (path, "%s/%s", dir, *file);
		if (ret < 0)
			continue;

		f = fopen (path, "r");
		if (! f)
			continue;

		while (fgets (buffer, sizeof (buffer), f)) {
			len = strlen (buffer);
			buffer[len-1] = '\0';
			show ("%s=%s", *file, buffer);
		}

		fclose (f);
	}
}

char *
get_scheduler_name (int sched)
{
	struct procenv_map *p;

	for (p = scheduler_map; p && p->name; p++) {
		if (p->num == sched)
			return p->name;
	}

	return NULL;
}

void
show_linux_scheduler (void)
{
	int sched;

	sched = sched_getscheduler (0);
	if (sched < 0)
		die ("failed to query scheduler");

	show ("scheduler: %s", get_scheduler_name (sched));
}

void
show_linux_cpu (void)
{
	int cpu;
	long max;

	cpu = sched_getcpu ();
	if (cpu < 0)
		die ("failed to query cpu");

	/* adjust to make 1-based */
	cpu++;

	max = get_sysconf (_SC_NPROCESSORS_ONLN);

	show ("cpu: %u of %lu", cpu, max);
}

/**
 * get_root:
 *
 * @root [out]: path of root directory,
 * @len: Size of @root (should be atleast PATH_MAX).
 **/
void
get_root (char *root, size_t len)
{
	char self[] = "/proc/self/root";
	ssize_t bytes = readlink (self, root, len);
	assert (bytes != -1);
	root[bytes] = '\0';
}

#endif

bool
has_ctty (void)
{
	int fd;
	fd = open ("/dev/tty", O_RDONLY | O_NOCTTY);

	if (fd < 0)
		return FALSE;

	close (fd);

	return TRUE;
}

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
void
show_bsd_cpu (void)
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

	show ("cpu: %u of %lu", cpu, max);
}

void
get_bsd_misc (void)
{
	char                 errors[_POSIX2_LINE_MAX];
	kvm_t               *kvm;
	struct kinfo_proc   *proc;
	int                  ignored;

	kvm = kvm_openfiles (NULL, _PATH_DEVNULL, NULL, O_RDONLY, errors);
	if (! kvm)
		die ("unable to open kvm");

	proc = kvm_getprocs (kvm, KERN_PROC_PID, user.pid, &ignored);
	if (! proc)
		die ("failed to get process info");

	misc.in_jail = (proc->ki_flag & P_JAILED) ? TRUE : FALSE;
	strcpy (user.proc_name, proc->ki_comm);

	if (kvm_close (kvm) < 0)
		die ("failed to close kvm");
}

#endif

int
get_output_value (const char *name)
{
	struct procenv_map *p;

	assert (name);

	for (p = output_map; p && p->name; p++) {
		if (! strcmp (name, p->name)) {
			return p->num;
		}
	}
	die ("invalid output value: '%s'", name);
}

void
check_envvars (void)
{
	char   *e;
	char   *token;
	char   *string;
	size_t  count = 0;
	size_t  i;

	e = getenv (PROCENV_OUTPUT_ENV);
	if (e && *e) {
		output = get_output_value (e);
	}

	e = getenv (PROCENV_FILE_ENV);
	if (e && *e) {
		output_file = e;
		output = OUTPUT_FILE;
	}
	e = getenv (PROCENV_EXEC_ENV);
	if (e && *e) {
		char *tmp;

		string = strdup (e);
		if (! string)
			die ("failed to copy environment string");

		/* establish number of fields */
		for (tmp = string; tmp && *tmp; ) {
			tmp = index (tmp, ' ');
			if (tmp) {
				/* jump over matched char */
				tmp++;
				count++;
			}

		}

		/* allocate space for arguments.
		 * +1 for terminator.
		 */
		exec_args = calloc (count + 1, sizeof (char *));
		if (! exec_args)
			die ("failed to allocate space for args copy");

		/* build the argument array */
		tmp = string;
		for (token = strsep (&tmp, " "), i=0;
				token;
				token = strsep (&tmp, " "), i++)
		{
			exec_args[i] = strdup (token);
			if (! exec_args[i])
				die ("failed to allocate space for exec arg");
		}

		free (string);
	}
}

void
get_major_minor (const char *path, int *major, int *minor)
{
	struct stat  st;

	assert (path);
	assert (major);
	assert (minor);

	if (stat (path, &st) < 0) {
		/* Don't fail as this query may be for a mount which the
		 * user does not have permission to check.
		 */
		warn ("unable to stat path '%s'", path);
		*major = *minor = -1;
		return;
	}

	*major = major (st.st_dev);
	*minor = minor (st.st_dev);
}

/**
 * Find full path to @argv0.
 *
 * Returns: newly-allocated path to @argv0, or NULL on error.
 *
 * Note that returned path will not necessarily be the canonical path,
 * so it should be passed to readlink(2).
 **/
char *
get_path (const char *argv0)
{
    char        *slash;
    char        *path;
    char        *prog_path = NULL;
    char        *tmp;
    char        *element;
    char         possible[PATH_MAX];
    struct stat  statbuf;

    assert (argv0);

    slash = strchr (argv0, '/');

    if (slash == argv0) {
        /* absolute path */
        return strdup (argv0);
    } else if (slash) {
         char cwd[PATH_MAX];

         /* relative path */
         assert (getcwd (cwd, sizeof (cwd)));
         strcat (cwd, "/");
         strcat (cwd, argv0);

         if (! stat (cwd, &statbuf))
             return strdup (cwd);
         return NULL;
    }

    /* path search required */
    tmp = getenv ("PATH");
    path = strdup (tmp ? tmp : _PATH_STDPATH);
    assert (path);

    tmp = path;
    for(element = strsep (&tmp, ":");
            element;            
            element = strsep (&tmp, ":")) {

        sprintf (possible, "%s%s%s",
                element,
                element [strlen (element)-1] == '/' ? "" : "/",
                argv0);

        if (! stat (possible, &statbuf)) {
            prog_path = strdup (possible);
            break;
        }
    }

    free (path);

    return prog_path;
}

int
main (int  argc,
		char *argv[])
{
	char **args;
	int    ret;
	int    option;
	int    long_index;
	int    done = FALSE;

	struct option long_options[] = {
		{"meta"         , no_argument, NULL, 'a'},
		{"libs"         , no_argument, NULL, 'b'},
		{"cgroup"       , no_argument, NULL, 'c'},
		{"cgroups"      , no_argument, NULL, 'c'},
		{"compiler"     , no_argument, NULL, 'd'},
		{"env"          , no_argument, NULL, 'e'},
		{"environment"  , no_argument, NULL, 'e'},
		{"fds"          , no_argument, NULL, 'f'},
		{"sizeof"       , no_argument, NULL, 'g'},
		{"help"         , no_argument, NULL, 'h'},
		{"misc"         , no_argument, NULL, 'i'},
		{"uname"        , no_argument, NULL, 'j'},
		{"clock"        , no_argument, NULL, 'k'},
		{"clocks"       , no_argument, NULL, 'k'},
		{"limits"       , no_argument, NULL, 'l'},
		{"mount"        , no_argument, NULL, 'm'},
		{"mounts"       , no_argument, NULL, 'm'},
		{"confstr"      , no_argument, NULL, 'n'},
		{"oom"          , no_argument, NULL, 'o'},
		{"proc"         , no_argument, NULL, 'p'},
		{"process"      , no_argument, NULL, 'p'},
		{"time"         , no_argument, NULL, 'q'},
		{"range"        , no_argument, NULL, 'r'},
		{"ranges"       , no_argument, NULL, 'r'},
		{"signal"       , no_argument, NULL, 's'},
		{"signals"      , no_argument, NULL, 's'},
		{"tty"          , no_argument, NULL, 't'},
		{"stat"         , no_argument, NULL, 'u'},
		{"version"      , no_argument, NULL, 'v'},
		{"capabilities" , no_argument, NULL, 'w'},
		{"pathconf"     , no_argument, NULL, 'x'},
		{"sysconf"      , no_argument, NULL, 'y'},
		{"timezone"     , no_argument, NULL, 'z'},

		{"output"      , required_argument, NULL, 0},
		{"file"        , required_argument, NULL, 0},
		{"exec"        , no_argument      , NULL, 0},

		/* terminator */
		{NULL, no_argument, NULL, 0}
	};

	program_name = argv[0];

	/* Check before command-line options, since the latter
	 * must take priority.
	 */
	check_envvars ();

	init ();

	while (TRUE) {
		option = getopt_long (argc, argv,
				"abcdefghijklmnopqrstvwxyz",
				long_options, &long_index);
		if (option == -1)
			break;

		done = TRUE;

		selected_option = option;

		set_indent ();

		switch (option)
		{
		case 0:
			if (! strcmp ("output", long_options[long_index].name)) {
				output = get_output_value (optarg);
			} else if (! strcmp ("file", long_options[long_index].name)) {
				output = OUTPUT_FILE;
				output_file = optarg;
			} else if (! strcmp ("exec", long_options[long_index].name)) {
				reexec = TRUE;
			}
		
			done = FALSE;

			/* reset */
			selected_option = 0;
			set_indent ();

			break;

		case 'a':
			dump_meta ();
			break;

		case 'b':
			show_libs ();
			break;

		case 'c':
#if defined (PROCENV_LINUX)
			show_linux_cgroups ();
#endif
			break;

		case 'd':
			show_compiler ();
			break;

		case 'e':
			show_env ();
			break;

		case 'f':
			dump_fds ();
			break;

		case 'g':
			show_sizeof ();
			break;

		case 'h':
			usage ();
			break;

		case 'i':
			get_misc ();
			dump_misc ();
			break;

		case 'j':
			show_uname ();
			break;

		case 'k':
			show_clocks ();
			break;

		case 'l':
			show_rlimits ();
			break;

		case 'm':
			show_mounts (SHOW_MOUNTS);
			break;

		case 'n':
			show_confstrs ();
			break;

		case 'o':
#if defined (PROCENV_LINUX)
			show_oom ();
#endif
			break;

		case 'p':
			dump_user ();
			break;

		case 'q':
			show_time ();
			break;

		case 'r':
			show_ranges ();
			break;

		case 's':
			show_signals ();
			break;

		case 't':
			show_tty_attrs ();
			break;

		case 'u':
			show_stat ();
			break;

		case 'v':
			show ("%s %s: %s",
					PACKAGE_NAME,
					_("version"),
					PACKAGE_VERSION);
			/* FIXME: License */
#if 0
			wprintf (L"%s: %s\n", _("License"), PROGRAM_LICENSE);
#endif
			show ("%s: %s\n", _("Written by"), PROGRAM_AUTHORS);
			//exit (EXIT_SUCCESS);
			break;

		case 'w':
#if defined (PROCENV_LINUX)
			show_capabilities ();
#endif
			break;

		case 'x':
			show_mounts (SHOW_PATHCONF);
			break;

		case 'y':
			dump_sysconf ();
			break;

		case 'z':
			show_timezone ();
			break;

		case '?':
			die ("invalid option '%c' specified", option);
			break;
		}
	}

	if (done)
		exit (EXIT_SUCCESS);

	if (output == OUTPUT_SYSLOG)
		openlog (PACKAGE_NAME, LOG_CONS | LOG_PID, LOG_USER);

	if (reexec && ! exec_args && optind >= argc)
			die ("must specify atleast one argument with '--exec'");

	dump ();

	cleanup ();

	if (reexec) {
		if (! exec_args) {
			argv += optind;
			exec_args = argv;
		}

		execvp (exec_args[0], exec_args);
		die ("failed to re-exec %s", exec_args[0]);
	}

	exit (EXIT_SUCCESS);

}
