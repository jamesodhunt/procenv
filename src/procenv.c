/* TODO:
 * - FreeBSD support!
 */


/* FIXME: freebsd ipcs /data/svn/freebsd/head/usr.bin/ipcs/ipcs.c

kd = kvm_openfiles(namelist, core, NULL, O_RDONLY, kvmoferr);

kvm_nlist(kd, symbols)
kget(X_MSGINFO, &msginfo, sizeof(msginfo));
kget(X_SHMINFO, &shminfo, sizeof(shminfo));
struct shmid_kernel *kxshmids;
kget(X_SHMSEGS, kxshmids, kxshmids_len);
*/

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
 * Copyright 2012-2013 James Hunt.
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
 * doc:
 *
 * The output document.
 **/
char *doc = NULL;

/**
 * output:
 *
 * Where to send output.
 **/
Output output = OUTPUT_STDOUT;

/**
 * output_format:
 *
 * Format output will be displayed in.
 **/
OutputFormat output_format = OUTPUT_FORMAT_TEXT;

/**
 * output_file:
 *
 * Name or output file to send output to if not NULL.
 **/
const char *output_file = NULL;

/**
 * text_separator:
 *
 * Separator used for text output format to separate a name from a
 * value.
 **/
const char *text_separator = PROCENV_DEFAULT_TEXT_SEPARATOR;

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
 * Number of spaces to indent output.
 **/
int indent = 0;

/**
 * indent_amount:
 *
 * Number of INDENT_CHARs to emit for an indent.
 **/
int indent_amount = DEFAULT_INDENT_AMOUNT;

/**
 * indent_char:
 *
 * Character to use for indenting.
 **/
int indent_char = DEFAULT_INDENT_CHAR;

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

/**
 * Copy of argv and argc used by show_arguments().
 **/
char **argvp = NULL;
int argvc = 0;

/**
 * last_element: Type of previous element handled.
 **/
ElementType last_element = ELEMENT_TYPE_NONE;

/**
 * current_element: Type of element currently being handled.
 **/
ElementType current_element = ELEMENT_TYPE_NONE;

/**
 * in_container:
 *
 * TRUE if currently processing a container, else FALSE.
 **/
int in_container = FALSE;

struct procenv_user     user;
struct procenv_misc     misc;
struct procenv_priority priority;

struct utsname uts;

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

	{ 0, NULL }
};
#endif

struct procenv_map output_map[] = {
	{ OUTPUT_FILE   , "file" },
	{ OUTPUT_STDERR , "stderr" },
	{ OUTPUT_STDOUT , "stdout" },
	{ OUTPUT_SYSLOG , "syslog" },
	{ OUTPUT_TERM   , "terminal" },

	{ 0, NULL }
};

struct procenv_map output_format_map[] = {
	{ OUTPUT_FORMAT_TEXT, "text" },
	{ OUTPUT_FORMAT_JSON, "json" },
	{ OUTPUT_FORMAT_XML, "xml" },

	{ 0, NULL }
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

struct if_flag_map {
	unsigned int  flag;
	char         *name;
} if_flag_map[] = {
	mk_map_entry (IFF_UP),
	mk_map_entry (IFF_BROADCAST),
	mk_map_entry (IFF_DEBUG),
	mk_map_entry (IFF_LOOPBACK),
	mk_map_entry (IFF_POINTOPOINT),
	mk_map_entry (IFF_RUNNING),
	mk_map_entry (IFF_NOARP),
	mk_map_entry (IFF_PROMISC),

#if defined (PROCENV_LINUX)
	mk_map_entry (IFF_NOTRAILERS),
#endif

	mk_map_entry (IFF_ALLMULTI),

#if defined (PROCENV_LINUX)
	mk_map_entry (IFF_MASTER),
	mk_map_entry (IFF_SLAVE),
#endif

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	mk_map_entry (IFF_SIMPLEX),
#endif

	mk_map_entry (IFF_MULTICAST),

#if defined (PROCENV_LINUX)
	mk_map_entry (IFF_PORTSEL),
	mk_map_entry (IFF_AUTOMEDIA),
	mk_map_entry (IFF_DYNAMIC),
	mk_map_entry (IFF_LOWER_UP),
	mk_map_entry (IFF_DORMANT),
	mk_map_entry (IFF_ECHO),
#endif

	{ 0, NULL }
};

/*
 * Note the gross hack to avoid need for flexible arrays.
 */
TranslateTable translate_table[] = {
	{
		OUTPUT_FORMAT_XML,
		{
			{ '\'', "&apos;" },
			{ '"', "&quot;" },
			{ '&', "&amp;" },
			{ '<', "&lt;" },
			{ '>', "&gt;" },

			/* terminator */
			{ '\0', NULL }
		}
	},
	{
		OUTPUT_FORMAT_JSON,
		{
			{ '"', "\\\"" },
			{ '\\', "\\\\" },

			/* hack! */
			{ '\0', NULL },
			{ '\0', NULL },
			{ '\0', NULL },

			/* terminator */
			{ '\0', NULL }
		}
	},
};

#if defined (PROCENV_LINUX)
struct if_extended_flag_map {
	unsigned int  flag;
	char         *name;
} if_extended_flag_map[] = {
	mk_map_entry (IFF_802_1Q_VLAN),
	mk_map_entry (IFF_EBRIDGE),
	mk_map_entry (IFF_SLAVE_INACTIVE),
	mk_map_entry (IFF_MASTER_8023AD),
	mk_map_entry (IFF_MASTER_ALB),
	mk_map_entry (IFF_BONDING),
	mk_map_entry (IFF_SLAVE_NEEDARP),
	mk_map_entry (IFF_ISATAP),

	{ 0, NULL }
};
#endif


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
#if defined (_SC_HOST_NAME_MAX)
	mk_posix_sysconf_map_entry (HOST_NAME_MAX),
#endif
	mk_posix_sysconf_map_entry (LINE_MAX),
	mk_posix_sysconf_map_entry (LOGIN_NAME_MAX),
	mk_posix_sysconf_map_entry (OPEN_MAX),
	mk_posix_sysconf_map_entry (PAGESIZE),
	mk_posix_sysconf_map_entry (RE_DUP_MAX),
	mk_posix_sysconf_map_entry (STREAM_MAX),
#if defined (_SC_SYMLOOP_MAX)
	mk_posix_sysconf_map_entry (SYMLOOP_MAX),
#endif
	mk_posix_sysconf_map_entry (TTY_NAME_MAX),
	mk_posix_sysconf_map_entry (TZNAME_MAX),
	{ _SC_VERSION, "_POSIX_VERSION(_SC_VERSION)" },
#if defined (_SC_POSIX2_C_DEV)
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

#if defined (_SC_ADVISORY_INFO)
	mk_posixopt_sysconf_map_entry (ADVISORY_INFO),
#endif
	mk_posixopt_sysconf_map_entry (ASYNCHRONOUS_IO),
#if defined (_SC_BARRIERS)
	mk_posixopt_sysconf_map_entry (BARRIERS),
#endif
#if defined (_POSIX_CHOWN_RESTRICTED)
	mk_sysconf_map_entry (_POSIX_CHOWN_RESTRICTED),
#endif
#if defined (_SC_CLOCK_SELECTION)
	mk_posixopt_sysconf_map_entry (CLOCK_SELECTION),
#endif
#if defined (_SC_CPUTIME)
	mk_posixopt_sysconf_map_entry (CPUTIME),
#endif
#if defined (_SC_FILE_LOCKING)
	mk_posixopt_sysconf_map_entry (FILE_LOCKING),
#endif
	mk_posixopt_sysconf_map_entry (FSYNC),
	mk_posixopt_sysconf_map_entry (JOB_CONTROL),
	mk_posixopt_sysconf_map_entry (MAPPED_FILES),
	mk_posixopt_sysconf_map_entry (MEMLOCK),
	mk_posixopt_sysconf_map_entry (MEMLOCK_RANGE),
	mk_posixopt_sysconf_map_entry (MEMORY_PROTECTION),
	mk_posixopt_sysconf_map_entry (MESSAGE_PASSING),
#if defined (_SC_MONOTONIC_CLOCK)
	mk_posixopt_sysconf_map_entry (MONOTONIC_CLOCK),
#endif
#ifdef _SC_MULTI_PROCESS
	mk_posixopt_sysconf_map_entry (MULTI_PROCESS),
#endif
	mk_posixopt_sysconf_map_entry (PRIORITIZED_IO),
	mk_posixopt_sysconf_map_entry (PRIORITY_SCHEDULING),
#if defined (_POSIX_RAW_SOCKETS)
	mk_sysconf_map_entry (_POSIX_RAW_SOCKETS),
#endif
#if defined (_SC_READER_WRITER_LOCKS)
	mk_posixopt_sysconf_map_entry (READER_WRITER_LOCKS),
#endif
	mk_posixopt_sysconf_map_entry (REALTIME_SIGNALS),
#if defined (_SC_REGEXP)
	mk_posixopt_sysconf_map_entry (REGEXP),
#endif
	mk_posixopt_sysconf_map_entry (SAVED_IDS),
	mk_posixopt_sysconf_map_entry (SEMAPHORES),
	mk_posixopt_sysconf_map_entry (SHARED_MEMORY_OBJECTS),
#if defined (_SC_SHELL)
	mk_posixopt_sysconf_map_entry (SHELL),
#endif
#if defined (_SC_SPAWN)
	mk_posixopt_sysconf_map_entry (SPAWN),
#endif
#if defined (_SC_SPIN_LOCKS)
	mk_posixopt_sysconf_map_entry (SPIN_LOCKS),
#endif
#if defined (_SC_SPORADIC_SERVER)
	mk_posixopt_sysconf_map_entry (SPORADIC_SERVER),
#endif
	mk_posixopt_sysconf_map_entry (SYNCHRONIZED_IO),
	mk_posixopt_sysconf_map_entry (THREAD_ATTR_STACKSIZE),
#if defined (_SC_THREAD_CPUTIME)
	mk_posixopt_sysconf_map_entry (THREAD_CPUTIME),
#endif
	mk_posixopt_sysconf_map_entry (THREAD_PRIO_INHERIT),
	mk_posixopt_sysconf_map_entry (THREAD_PRIO_PROTECT),
	mk_posixopt_sysconf_map_entry (THREAD_PRIORITY_SCHEDULING),
#if defined (_SC_THREAD_PROCESS_SHARED)
	mk_posixopt_sysconf_map_entry (THREAD_PROCESS_SHARED),
#endif
	mk_posixopt_sysconf_map_entry (THREAD_SAFE_FUNCTIONS),
#if defined (_SC_THREAD_SPORADIC_SERVER)
	mk_posixopt_sysconf_map_entry (THREAD_SPORADIC_SERVER),
#endif
	mk_posixopt_sysconf_map_entry (THREADS),
#if defined (_SC_TIMEOUTS)
	mk_posixopt_sysconf_map_entry (TIMEOUTS),
#endif
	mk_posixopt_sysconf_map_entry (TIMERS),
#if defined (_SC_TRACE)
	mk_posixopt_sysconf_map_entry (TRACE),
#endif
#if defined (_SC_TRACE_EVENT_FILTER)
	mk_posixopt_sysconf_map_entry (TRACE_EVENT_FILTER),
#endif
#if defined (_SC_TRACE_INHERIT)
	mk_posixopt_sysconf_map_entry (TRACE_INHERIT),
#endif
#if defined (_SC_TRACE_LOG)
	mk_posixopt_sysconf_map_entry (TRACE_LOG),
#endif
#ifdef _SC_TYPED_MEMORY_OBJECT
	mk_posixopt_sysconf_map_entry (TYPED_MEMORY_OBJECT),
#endif
#if defined (_POSIX_VDISABLE)
	mk_sysconf_map_entry (_POSIX_VDISABLE),
#endif
	mk_sysconf_map_entry (_XOPEN_CRYPT),
	mk_sysconf_map_entry (_XOPEN_LEGACY),
#if defined (_XOPEN_REALTIME)
	mk_sysconf_map_entry (_XOPEN_REALTIME),
#endif
#if defined (_XOPEN_REALTIME_THREADS)
	mk_sysconf_map_entry (_XOPEN_REALTIME_THREADS),
#endif
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
#ifdef SIGSTKFLT
	mk_map_entry (SIGSTKFLT),
#endif
#endif

	mk_map_entry (SIGSYS),

#if defined (PROCENV_LINUX)
#ifdef SIGUNUSED
	mk_map_entry (SIGUNUSED),
#endif
#endif
	mk_map_entry (SIGURG),
	mk_map_entry (SIGVTALRM),
	mk_map_entry (SIGWINCH),
	mk_map_entry (SIGXCPU),
	mk_map_entry (SIGXFSZ),

#if defined (PROCENV_BSD) || defined (PROCENV_HURD)
	mk_map_entry (SIGEMT),
	mk_map_entry (SIGINFO),
#endif

#if defined (PROCENV_HURD)
	mk_map_entry (SIGLOST),
#endif

	{ 0, NULL },
};

struct procenv_map locale_map[] = {

  /* The non-conditional ones are POSIX.  */
#ifdef LC_ADDRESS
	mk_map_entry (LC_ADDRESS),
#endif
	mk_map_entry (LC_COLLATE),
	mk_map_entry (LC_CTYPE),
#ifdef LC_IDENTIFICATION
	mk_map_entry (LC_IDENTIFICATION),
#endif
#ifdef LC_MEASUREMENT
	mk_map_entry (LC_MEASUREMENT),
#endif
	mk_map_entry (LC_MESSAGES),
	mk_map_entry (LC_MONETARY),
#ifdef LC_NAME
	mk_map_entry (LC_NAME),
#endif
	mk_map_entry (LC_NUMERIC),
#ifdef LC_PAPER
	mk_map_entry (LC_PAPER),
#endif
#ifdef LC_TELEPHONE
	mk_map_entry (LC_TELEPHONE),
#endif
	mk_map_entry (LC_TIME),

	{ 0, NULL }
};

struct procenv_map scheduler_map[] = {

	mk_map_entry (SCHED_OTHER),
	mk_map_entry (SCHED_FIFO),
	mk_map_entry (SCHED_RR),
#if defined (PROCENV_LINUX) && ! defined (PROCENV_ANDROID)
	mk_map_entry (SCHED_BATCH),
#ifdef SCHED_IDLE
	mk_map_entry (SCHED_IDLE),
#endif
#endif

	{ 0, NULL }
};

struct procenv_map thread_sched_policy_map[] = {
	mk_map_entry (SCHED_OTHER),
	mk_map_entry (SCHED_FIFO),
	mk_map_entry (SCHED_RR)
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
	show ("  -A, --arguments     : Display program arguments.");
	show ("  -b, --libs          : Display library details.");
	show ("  -c, --cgroups       : Display cgroup details (Linux only).");
	show ("  -C, --cpu           : Display CPU and scheduler details.");
	show ("  -d, --compiler      : Display compiler details.");
	show ("  -e, --environment   : Display environment variables.");
	show ("  -E, --semaphores    : Display semaphore details.");
	show ("  --exec              : Treat non-option arguments as program to execute.");
	show ("  -f, --fds           : Display file descriptor details.");
	show ("  --file=<file>       : Send output to <file> (implies --output=file).");
	show ("  --format=<format>   : Specify output format. <format> can be one of:");
	show ("");
	show ("                      json     : JSON output.");
	show ("                      text     : ASCII output (default).");
	show ("                      xml      : XML output.");
	show ("");
	show ("  -g, --sizeof        : Display sizes of data types in bytes.");
	show ("  -h, --help          : This help text.");
	show ("  -i, --misc          : Display miscellaneous details.");
	show ("  --indent            : Number of indent characters to use for each indent");
	show ("                        (default=%d).", DEFAULT_INDENT_AMOUNT);
	show ("  --indent-char=<c>   : Use character '<c>' for indenting");
	show ("                        (default='%c').", DEFAULT_INDENT_CHAR);
	show ("  -j, --uname         : Display uname details.");
	show ("  -k, --clocks        : Display clock details.");
	show ("  -l, --limits        : Display limits.");
	show ("  -L, --locale        : Display locale details.");
	show ("  -m, --mounts        : Display mount details.");
	show ("  -M, --messagequeues : Display message queue details.");
	show ("  -n, --confstr       : Display confstr details.");
	show ("  -N, --network       : Display network details.");
	show ("  -o, --oom           : Display out-of-memory manager details (Linux only)");
	show ("  --output=<type>     : Send output to alternative location.");
	show ("                        <type> can be one of:");
	show ("");
	show ("                      file     : Send output to a file.");
	show ("                      stderr   : Write to standard error.");
	show ("                      stdout   : Write to standard output (default).");
	show ("                      syslog   : Write to the system log file.");
	show ("                      terminal : Write to terminal.");
	show ("");
	show ("  -p, --process       : Display process details.");
	show ("  -P, --platform      : Display platform details.");
	show ("  -q, --time          : Display time details.");
	show ("  -r, --ranges        : Display range of data types.");
	show ("  --separator=<str>   : Specify string '<str>' as alternate delimiter");
	show ("                        for text format output (default='%s').",
			PROCENV_DEFAULT_TEXT_SEPARATOR);
	show ("  -s, --signals       : Display signal details.");
	show ("  -S, --sharedmemory  : Display shared memory details.");
	show ("  -t, --tty           : Display terminal details.");
	show ("  -T, --threads       : Display thread details.");
	show ("  -u, --stat          : Display stat details.");
	show ("  -U, --rusage        : Display rusage details.");
	show ("  -v, --version       : Display version details.");
	show ("  -w, --capabilities  : Display capaibility details (Linux only).");
	show ("  -x, --pathconf      : Display pathconf details.");
	show ("  -y, --sysconf       : Display sysconf details.");
	show ("  -z, --timezone      : Display timezone details.");
	show ("");
	show ("Notes:");
	show ("");
	show ("  - If no display option is specified, all details are displayed.");
	show ("  - Only one display option may be specified.");
	show ("  - All indent-char values are literal except '\\t' which can be");
	show ("    used to specify tab-indenting.");
	show ("  - Specifying a visible indent-char is only (vaguely) meaningful");
	show ("    for text output.");
	show ("");
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

	show_pathconf (what, dir, _PC_LINK_MAX);
	show_pathconf (what, dir, _PC_MAX_CANON);
	show_pathconf (what, dir, _PC_MAX_INPUT);
	show_pathconf (what, dir, _PC_NAME_MAX);
	show_pathconf (what, dir, _PC_PATH_MAX);
	show_pathconf (what, dir, _PC_PIPE_BUF);
	show_pathconf (what, dir, _PC_CHOWN_RESTRICTED);
	show_pathconf (what, dir, _PC_NO_TRUNC);
	show_pathconf (what, dir, _PC_VDISABLE);

	footer ();
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
 * @fmt: printf-style format with associated arguments that comprises
 *  the value part.
 *
 * Write output to @string, indented by @indent spaces. A trailing newline
 * will be added.
 *
 * Note that error scenarios cannot call die() as by definition output
 * may not be possible.
 **/
void
_show (const char *prefix, int indent, const char *fmt, ...)
{
	va_list   ap;
	char     *buffer = NULL;

	assert (fmt);

	if (indent)
		appendf (&buffer, "%*c", indent, indent_char);

	if (prefix && *prefix)
		appendf (&buffer, "%s: ", prefix);

	va_start (ap, fmt);
	appendva (&buffer, fmt, ap);
	va_end (ap);

	append (&buffer, "\n");

	_show_output (buffer);

	free (buffer);
}

/**
 * entry:
 *
 * @name: name of thing to display,
 * @fmt: printf-style format with associated arguments that comprises
 *  the value part.
 *
 * Add name/value pair represented by @name and value comprising
 * printf-format string to the @doc global. The value added will be
 * indented appropriately.
 **/
void
entry (const char *name, const char *fmt, ...)
{
	va_list   ap;
	char     *encoded_name = NULL;
	char     *encoded_value = NULL;

	assert (name);
	assert (fmt);

	common_assert ();

	if (output_format == OUTPUT_FORMAT_JSON && in_container)
		object_open (FALSE);

	change_element (ELEMENT_TYPE_ENTRY);

	encoded_name = strdup (name);
	assert (encoded_name);

	if (encode_string (&encoded_name) < 0)
		die ("failed to encode name");

	va_start (ap, fmt);
	appendva (&encoded_value, fmt, ap);
	va_end (ap);

	if (encode_string (&encoded_value) < 0)
		die ("failed to encode value");

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		appendf (&doc, "%s%s%s",
				encoded_name,
				text_separator,
				encoded_value);
		break;

	case OUTPUT_FORMAT_JSON:
		appendf (&doc, "\"%s\" : \"%s\"",
				encoded_name,
				encoded_value);
		break;

	case OUTPUT_FORMAT_XML:
		appendf (&doc, "<entry name=\"%s\">%s</entry>",
				encoded_name, encoded_value);
		break;

	default:
		assert_not_reached ();
		break;
	}

	free (encoded_name);
	free (encoded_value);

	if (output_format == OUTPUT_FORMAT_JSON && in_container)
		object_close (FALSE);
}

/**
 * _show_output:
 *
 * @string: String to display.
 *
 * Write output @string to appropriate location based on Output
 * destination.
 **/
void
_show_output (const char *string)
{
	int ret;

	if (! string || ! *string)
		return;

	switch (output) {
	case OUTPUT_SYSLOG:
		syslog (LOG_INFO, "%s", string);
		ret = 0;
		break;

	case OUTPUT_STDOUT:
		ret = fputs (string, stdout);
		break;

	case OUTPUT_STDERR:
		ret = fputs (string, stderr);
		break;

	case OUTPUT_TERM:
		assert (user.tty_fd != -1);
		ret = write (user.tty_fd, string, strlen (string));
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
		ret = write (output_fd, string, strlen (string));
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

	if (ret < 0) {
		fprintf (stderr, "ERROR: failed to output message\n");
		exit (EXIT_FAILURE);
	}
}

/**
 * inc_indent:
 *
 * Increase indent.
 **/
void
inc_indent (void)
{
	assert (indent >= 0);

	indent += indent_amount;
}

/**
 * dec_indent:
 *
 * Decrease indent.
 **/
void
dec_indent (void)
{
	assert (indent >= 0);

	indent -= indent_amount;

	assert (indent >= 0);
}

/**
 * add_indent:
 *
 * Insert the current indent to the output document.
 **/
void
add_indent (char **doc)
{
	common_assert ();

	if (! indent)
		return;

	if (indent_char == DEFAULT_INDENT_CHAR) {
		appendf (doc, "%*c", indent, indent_char);
	} else {
		char *buffer = NULL;

		appendf (&buffer, "%*c", indent, DEFAULT_INDENT_CHAR);

		/* Replace the default characters with the chosen character.
		 * Necessary as printf-type functions don't allow the padding
		 * character to be specified.
		 */
		memset (buffer, indent_char, strlen (buffer));

		append (doc, buffer);
		free (buffer);
	}
}

/**
 * master_header:
 *
 * @doc: document to write footer to.
 *
 * Main header which is displayed once.
 **/
void
master_header (char **doc)
{
	common_assert ();

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_JSON:
		object_open (FALSE);
		break;

	case OUTPUT_FORMAT_XML:
		append (doc, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		appendf (doc, "<%s version=\"%s\" package_string=\"%s\" "
				"mode=\"%s%s\" format_version=\"%d\">\n",
				PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_STRING,
				user.euid ? _(NON_STR) "-" : "",
				PRIVILEGED_STR,
				PROCENV_FORMAT_VERSION);

		inc_indent ();

		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * master_footer:
 *
 * @doc: document to write footer to.
 *
 * Main footer which is displayed once.
 **/
void
master_footer (char **doc)
{
    common_assert ();

    switch (output_format) {

        case OUTPUT_FORMAT_TEXT:
            /* Tweak */
	    append (doc, "\n");
            break;

        case OUTPUT_FORMAT_JSON:
            object_close (FALSE);

            /* Tweak */
            append (doc, "\n");
            break;

        case OUTPUT_FORMAT_XML:
            /* Tweak */
            append (doc, "\n");
            dec_indent ();
            appendf (doc, "</%s>\n", PACKAGE_NAME);
            break;

        default:
            assert_not_reached ();
            break;
    }
}

/**
 * object_open:
 *
 * @retain: if TRUE, do not disrupt the current element such that the
 * opening of the object will be invisible to the state machine, but
 * will still produce the required output.
 *
 * Handle opening an object.
 **/
void
object_open (int retain)
{
	common_assert ();

	if (retain)
		format_element ();
	else
		change_element (ELEMENT_TYPE_OBJECT_OPEN);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_JSON:
		append (&doc, "{");
		break;

	case OUTPUT_FORMAT_XML:
		/* NOP */
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * object_close:
 *
 * @retain: if TRUE, do not disrupt the current element such that the
 * object closure will be invisible to the state machine, but will still
 * produce @retain: if TRUE, do not disrupt the current element.
 *
 * Handle closing an object.
 **/
void
object_close (int retain)
{
	common_assert ();

	if (retain)
		format_element ();
	else
		change_element (ELEMENT_TYPE_OBJECT_CLOSE);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_JSON:
		append (&doc, "}");
		break;

	case OUTPUT_FORMAT_XML:
		/* NOP */
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * section_open:
 *
 * @name: name of section.
 *
 * Start a new section which will contain >0 entry() calls.
 **/
void
section_open (const char *name)
{
	assert (name);
	common_assert ();

	change_element (ELEMENT_TYPE_SECTION_OPEN);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		appendf (&doc, "%s:", name);
		break;

	case OUTPUT_FORMAT_JSON:
		appendf (&doc, "\"%s\" : {", name);
		break;

	case OUTPUT_FORMAT_XML:
		appendf (&doc, "<section name=\"%s\">", name);
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
section_close (void)
{
	common_assert ();

	change_element (ELEMENT_TYPE_SECTION_CLOSE);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_JSON:
		append (&doc, "}");
		break;

	case OUTPUT_FORMAT_XML:
		append (&doc, "</section>");
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * container_open:
 *
 * @name: name of container.
 *
 * Start a new container which will contain >0 entry() calls.
 *
 * This is primarily to handle JSON arrays.
 **/
void
container_open (const char *name)
{
	assert (name);
	common_assert ();

	change_element (ELEMENT_TYPE_CONTAINER_OPEN);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		appendf (&doc, "%s:", name);
		break;

	case OUTPUT_FORMAT_JSON:
		appendf (&doc, "\"%s\" : [", name);
		break;

	case OUTPUT_FORMAT_XML:
		appendf (&doc, "<container name=\"%s\">", name);
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * container_close:
 *
 * Finish with a container.
 **/
void
container_close (void)
{
	common_assert ();

	change_element (ELEMENT_TYPE_CONTAINER_CLOSE);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		/* NOP */
		break;

	case OUTPUT_FORMAT_JSON:
		append (&doc, "]");
		break;

	case OUTPUT_FORMAT_XML:
		append (&doc, "</container>");
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
header (const char *name)
{
	assert (name);
	common_assert ();

	section_open (name);
}

void
footer (void)
{
	common_assert ();
	section_close ();
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
	int               container_disabled = FALSE;

	container_open ("signals");

	/* Query blocked signals.
	 *
	 * How should be 0, but valgrind complains.
	 */
	if (sigprocmask (SIG_BLOCK, NULL, &old_sigset) < 0)
		die ("failed to query signal mask");

	for (i = 1; i <= NUM_SIGNALS; i++) {
		const char *signal_name;
		const char *signal_desc;

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

		signal_name = get_signal_name (i);
		signal_desc = strsignal (i);

		if (output_format == OUTPUT_FORMAT_JSON &&
				in_container == TRUE) {
			/* Nasty hack to produce valid JSON */
			container_disabled = TRUE;
			in_container = FALSE;
			object_open (FALSE);
		}

		section_open (signal_name);

		entry ("number", "%d", i);
		entry ("description", "'%s'", signal_desc ? signal_desc : UNKNOWN_STR);
		entry ("blocked", "%s", blocked ? YES_STR : NO_STR);
		entry ("ignored", "%s", ignored ? YES_STR : NO_STR);

		section_close ();

		if (output_format == OUTPUT_FORMAT_JSON &&
				container_disabled == TRUE) {
			object_close (FALSE);

			/* re-enable */
			in_container = TRUE;
		}
	}

    container_close ();
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

#if defined (PROCENV_LINUX)

	if (LINUX_KERNEL_MMR (2, 6, 25)) {
#if defined (RLIMIT_RTTIME)
		show_limit (RLIMIT_RTTIME);
#endif
	}
	show_limit (RLIMIT_LOCKS);
#endif

	show_limit (RLIMIT_MEMLOCK);

#if defined (PROCENV_LINUX)

	if (LINUX_KERNEL_MMR (2, 6, 8)) {
#if defined (RLIMIT_MSGQUEUE)
		show_limit (RLIMIT_MSGQUEUE);
#endif
	}

	if (LINUX_KERNEL_MMR (2, 6, 12)) {
#if defined RLIMIT_NICE
		show_limit (RLIMIT_NICE);
#endif
	}

#endif

	show_limit (RLIMIT_NOFILE);
	show_limit (RLIMIT_NPROC);
	show_limit (RLIMIT_RSS);

#if defined (PROCENV_LINUX)
	show_limit (RLIMIT_RTPRIO);
#endif

#if defined (PROCENV_LINUX)

	if (LINUX_KERNEL_MMR (2, 6, 8)) {
#if defined (RLIMIT_SIGPENDING)
	show_limit (RLIMIT_SIGPENDING);
#endif
	}

#endif

	show_limit (RLIMIT_STACK);

	footer ();
}

void
show_rusage (void)
{
	struct rusage usage;

	if (getrusage (RUSAGE_SELF, &usage) < 0)
		die ("unable to query rusage");

	header ("rusage");

	show_usage (usage, ru_maxrss);
	show_usage (usage, ru_ixrss);
	show_usage (usage, ru_idrss);
	show_usage (usage, ru_isrss);
	show_usage (usage, ru_minflt);
	show_usage (usage, ru_majflt);
	show_usage (usage, ru_nswap);
	show_usage (usage, ru_inblock);
	show_usage (usage, ru_oublock);
	show_usage (usage, ru_msgsnd);
	show_usage (usage, ru_msgrcv);
	show_usage (usage, ru_nsignals);
	show_usage (usage, ru_nvcsw);
	show_usage (usage, ru_nivcsw);

    footer ();
}

void
show_sysconf (void)
{
	struct procenv_map *p;
	long                value;

	header ("sysconf");

	for (p = sysconf_map; p && p->name; p++) {
		value = get_sysconf (p->num);
		entry (p->name, "%ld", value);
	}

    footer ();
}

#ifndef PROCENV_ANDROID

void
show_confstrs (void)
{
	header ("confstr");

#if defined (_CS_GNU_LIBC_VERSION)
	show_confstr (_CS_GNU_LIBC_VERSION);
#endif
#if defined (_CS_GNU_LIBPTHREAD_VERSION)
	show_confstr (_CS_GNU_LIBPTHREAD_VERSION);
#endif
	show_confstr (_CS_PATH);

	footer ();
}

#endif

void
get_misc (void)
{
	misc.umask_value = umask (S_IWGRP|S_IWOTH);
	(void)umask (misc.umask_value);
	assert (getcwd (misc.cwd, sizeof (misc.cwd)));

#if defined (PROCENV_LINUX)
	get_canonical (ROOT_PATH, misc.root, sizeof (misc.root));
#endif
#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	get_bsd_misc ();
#endif
}

#if !defined (PROCENV_HURD)
/**
 * is_console:
 * @fd: open file descriptor.
 *
 * Check if specified file descriptor is attached to a _console_
 * device (physical or virtual).
 *
 * Notes:
 *   - ptys are NOT consoles :)
 *   - running inside screen/tmux will report not running on console.
 *
 * Returns: TRUE if @fd is attached to a console, else FALSE.
 **/
int
is_console (int fd)
{
	struct vt_mode  vt;
	int             ret;

	ret = ioctl (fd, VT_GETMODE, &vt);

	return !ret;
}
#endif

void
show_proc (void)
{
	header ("process");

	entry ("process id (pid)", "%d", user.pid);

	entry ("parent process id (ppid)", "%d", user.ppid);
	entry ("session id (sid)", "%d (leader=%s)",
			user.sid,
			is_session_leader () ? YES_STR : NO_STR);

	entry ("name", "'%s'", user.proc_name);

	show_proc_branch ();

	entry ("process group id", "%d (leader=%s)",
			user.pgroup,
			is_process_group_leader () ? YES_STR : NO_STR);
	
	entry ("foreground process group", "%d", user.fg_pgroup);

	entry ("terminal", "'%s'", user.ctrl_terminal);

	entry ("has controlling terminal", "%s",
			has_ctty () ? YES_STR : NO_STR);

#if defined (PROCENV_HURD)
	entry ("on console", "%s", UNKNOWN_STR);
#else
	entry ("on console", "%s",
			is_console (user.tty_fd) ? YES_STR : NO_STR);
#endif

	entry ("real user id (uid)", "%d ('%s')",
			user.uid,
			get_user_name (user.uid));

	entry ("effective user id (euid)", "%d ('%s')",
			user.euid,
			get_user_name (user.euid));

	entry ("saved set-user-id (suid)", "%d ('%s')",
			user.suid,
			get_user_name (user.suid));

	entry ("real group id (gid)", "%d ('%s')",
			user.gid,
			get_group_name (user.gid));

	entry ("effective group id (egid)", "%d ('%s')",
			user.egid,
			get_group_name (user.egid));

	entry ("saved set-group-id (sgid)", "%d ('%s')",
			user.sgid,
			get_group_name (user.sgid));

	entry ("login name", "'%s'", user.login ? user.login : "");

	section_open ("passwd");

	entry ("name", "'%s'", user.passwd.pw_name);

#if ! defined (PROCENV_ANDROID)
	/* No gecos on Android. In fact it doesn't actually use the
	 * passwd database, but meh.
	 */
	entry ("gecos", "'%s'", user.passwd.pw_gecos);
#endif

	entry ("dir", "'%s'", user.passwd.pw_dir);
	entry ("shell", "'%s'", user.passwd.pw_shell);

	section_close ();

	show_all_groups ();

	footer ();
}

void
show_priorities (void)
{
#if defined (PROCENV_LINUX)
	int sched;

	sched = sched_getscheduler (0);
#endif

	section_open ("scheduler");

#if defined (PROCENV_LINUX)
	entry ("type", "%s",
			sched < 0 ? UNKNOWN_STR :
			get_scheduler_name (sched));
#endif

	section_open ("priority");

	entry ("process", "%d", priority.process);
	entry ("process group", "%d", priority.pgrp);
	entry ("user", "%d", priority.user);

	section_close ();

	section_close ();
}

void
show_misc (void)
{
	header ("misc");

	entry ("umask", "%4.4o", misc.umask_value);
	entry ("current directory (cwd)", "'%s'", misc.cwd);
#if defined (PROCENV_LINUX)
	entry ("root", "%s%s%s",
			strcmp (misc.root, UNKNOWN_STR) ? "'" : "",
			misc.root,
			strcmp (misc.root, UNKNOWN_STR) ? "'" : "");
#endif
	entry ("chroot", "%s", in_chroot () ? YES_STR : NO_STR);
	entry ("container", "%s", container_type ());

#if defined (PROCENV_LINUX)
	show_linux_prctl ();

	section_open ("linux security module");
	show_linux_security_module ();
	show_linux_security_module_context ();
	section_close ();
#endif

	entry ("memory page size", "%d bytes", getpagesize ());

#if defined (PROCENV_LINUX)
#ifdef LINUX_VERSION_CODE
	entry ("kernel headers version", "%u.%u.%u",
			(LINUX_VERSION_CODE >> 16),
			((LINUX_VERSION_CODE >> 8) & 0xFF),
			(LINUX_VERSION_CODE & 0xFF));
#endif
#endif

	footer ();
}

void
show_platform (void)
{
	long kernel_bits;
	long executable_bits;

	header ("platform");

	entry ("operating system", "%s", get_os ());
	entry ("architecture", "%s", get_arch ());

	kernel_bits = get_kernel_bits ();

	executable_bits = sizeof (void *) * CHAR_BIT * sizeof (char);

	if (kernel_bits == -1)
		entry ("kernel bits", "%s", UNKNOWN_STR);
	else
		entry ("kernel bits", "%lu", kernel_bits);

	entry ("executable bits", "%lu", executable_bits);

	entry ("code endian", "%s",
		is_big_endian () ? BIG_STR : LITTLE_STR);

	show_data_model ();

    footer ();
}

void
show_cpu (void)
{
	header ("cpu");

#if defined (PROCENV_LINUX)
	show_linux_cpu ();
#endif

#if defined (PROCENV_BSD)
	show_bsd_cpu ();
#endif
	show_priorities ();

	footer ();
}

void
show_fds (void)
{
#if defined (PROCENV_LINUX)
	show_fds_linux ();
#else
	show_fds_generic ();
#endif

}

void
show_fds_generic (void)
{
	int fd;
	int max;

	container_open ("file descriptors");

	max = sysconf (_SC_OPEN_MAX);

	for (fd = 0; fd < max; fd++) {
		int    is_tty = isatty (fd);
		char  *name = NULL;
		char  *num = NULL;

		if (! fd_valid (fd))
			continue;

#if ! defined (PROCENV_ANDROID)
		name = ttyname (fd);
#endif
		appendf (&num, "%d", fd);

		section_open (num);

		entry ("terminal", "%s", is_tty ? YES_STR : NO_STR);
		entry ("valid", "%s", fd_valid (fd) ? YES_STR : NO_STR);
		entry ("device", "%s", name ? name : NA_STR);

		section_close ();

		free (num);
	}

	container_close ();
}

void
show_env (void)
{
	char    **env = environ;
	char     *name;
	char     *value;
	size_t    i;

	header ("environment");

	/* Calculate size of environment array */
	for (i=0; env[i]; i++)
		;

	/* sort it */
	qsort (env, i, sizeof (env[0]), qsort_compar);

	env = environ;
	while (env && *env) {
		name = *env;
		value = strchr (name, '=');
		assert (value);
		*value = '\0';
		value++;

		entry (name, "%s", value);
		env++;
	}

	footer ();
}

int
qsort_compar (const void *a, const void *b)
{
	return strcoll (*(char * const *)a, *(char * const *)b);
}

void
get_user_info (void)
{
	struct passwd *pw;
	void          *p;
	int            ret;

	user.pid  = getpid ();
	user.ppid = getppid ();

#if defined (PROCENV_LINUX)
	if (LINUX_KERNEL_MMR (2, 6, 11)) {
		if (prctl (PR_GET_NAME, user.proc_name, 0, 0, 0) < 0)
			strcpy (user.proc_name, UNKNOWN_STR);
	}
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

#if defined (PROCENV_ANDROID)
	sprintf (user.ctrl_terminal, "/dev/tty");
#else
	ctermid (user.ctrl_terminal);
#endif

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

/**
 * appendn:
 *
 * @str: [output] string to append to,
 * @new: string to append to @str,
 * @len: length of @new.
 *
 * Append first @len bytes of @new to @str,
 * ensuring result is nul-terminated.
 **/
void
appendn (char **str, const char *new, size_t len)
{
	size_t  total;

	assert (str);
	assert (new);

	if (! len)
		return;

	if (! *str)
		*str = strdup ("");

	/* +1 for terminating nul */
	total = strlen (*str) + 1;

	total += len;

	*str = realloc (*str, total);
	assert (*str);

	strncat (*str, new, len);

	assert ((*str)[total-1] == '\0');
}

/* append @new to @str */
void
append (char **str, const char *new)
{
	size_t  len;
	assert (str);
	assert (new);

	len = strlen (new);

	appendn (str, new, len);
}

/* append @fmt and args to @str */
void
appendf (char **str, const char *fmt, ...)
{
    va_list   ap;
    char     *new = NULL;

    assert (str);
    assert (fmt);

    if (! *str)
	    *str = strdup ("");

    assert (*str);

    va_start (ap, fmt);

    if (vasprintf (&new, fmt, ap) < 0) {
        perror ("vasprintf");
        exit (EXIT_FAILURE);
    }

    va_end (ap);

    append (str, new);
    free (new);
}

/* append @fmt and args to @str */
void
appendva (char **str, const char *fmt, va_list ap)
{
    char  *new = NULL;

    assert (str);
    assert (fmt);

    if (! *str)
	    *str = strdup ("");

    assert (*str);

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
	char   *str = NULL;

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

	size = ret;

	if (size == 0) {
		char *group;

		free (groups);

		group = get_group_name (user.passwd.pw_gid);
		if (! group) {
			entry ("groups", "%s", UNKNOWN_STR);
			return;
		}

		appendf (&str, " '%s' (%d)",
				group,
				user.passwd.pw_gid);
		entry ("groups", "%s", str);
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

	qsort (group_names, size, sizeof (char *), qsort_compar);

	for (i = 0; i < size; i++) {
		if (i+1 == size)
			appendf (&str, "%s", group_names[i]);
		else
			appendf (&str, "%s, ", group_names[i]);
		free (group_names[i]);
	}
	free (group_names);
	free (groups);

	entry ("groups", "%s", str);
	free (str);

	return;

error:
	die ("failed to allocate space for groups");
}

void
init (void)
{
	/* required to allow for more graceful handling of prctl(2)
	 * options that were introduced in kernel version 'x.y'.
	 */
	get_uname ();

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

	free (doc);
}

/**
 * is_big_endian:
 *
 * Returns: TRUE if system is big-endian, else FALSE.
 **/
bool
is_big_endian (void)
{
	int x = 1;

	if (*(char *)&x == 1)
		return FALSE;

	return TRUE;
}

void
show_meta (void)
{
	header ("meta");

	entry ("version", "%s", PACKAGE_VERSION);
	entry ("package", "%s", PACKAGE_STRING);
	entry ("mode", "%s%s",
			user.euid ? _(NON_STR) "-" : "",
			PRIVILEGED_STR);
	entry ("format-version", "%d", PROCENV_FORMAT_VERSION);

	footer ();
}

void
show_arguments (void)
{
	int  i;

	header ("arguments");

	entry ("count", "%u", argvc);

	container_open ("list");

	for (i = 0; i < argvc; i++) {
		char  *buffer = NULL;

		appendf (&buffer, "argv[%d]", i);

		entry (buffer, "%s", argvp[i]);
		free (buffer);
	}

	container_close ();

	footer ();
}

void
show_stat (void)
{
	struct stat  st;
	char         real_path[PATH_MAX];
	char         formatted_atime[32];
	char         formatted_ctime[32];
	char         formatted_mtime[32];
	char        *modestr;
	mode_t       perms;
	char        *tmp = NULL;

	assert (program_name);
	assert (misc.cwd);

	header ("stat");

	tmp = get_path (program_name);
	assert (tmp);

	if (! realpath (tmp, real_path))
		die ("unable to resolve path");

	free (tmp);

	if (stat (real_path, &st) < 0)
		die ("unable to stat path: '%s'", real_path);

	header ("stat");

	entry ("argv[0]", "'%s'", program_name);
	entry ("real path", "'%s'", real_path);

	section_open ("device");

	entry ("major", "%u", major (st.st_dev));
	entry ("minor", "%u", minor (st.st_dev));

	section_close ();

	entry ("inode", "%lu", (unsigned long int)st.st_ino);

	modestr = format_perms (st.st_mode);
	if (! modestr)
		die ("failed to allocate space for permissions string");
	perms = (st.st_mode &= ~S_IFMT);

	if (perms & S_ISUID)
		modestr[3] = 's';
	if (perms & S_ISGID)
		modestr[6] = 's';
	if (perms & S_ISVTX)
		modestr[9] = 't';

	section_open ("permissions");

	entry ("octal", "%4.4o", perms);
	entry ("symbolic", "%s", modestr);
	free (modestr);

	section_close ();

	entry ("hard links", "%u", st.st_nlink);
	entry ("user id (uid)", "%d ('%s')", st.st_uid, get_user_name (st.st_uid));
	entry ("group id (gid)", "%d ('%s')", st.st_gid, get_group_name (st.st_uid));
	entry ("size", "%lu bytes (%lu 512-byte blocks)", st.st_size, st.st_blocks);

	/*****************************************/
	section_open ("times");

	if (! ctime_r ((time_t *)&st.st_atime, formatted_atime))
		die ("failed to format atime");
	formatted_atime[ strlen (formatted_atime)-1] = '\0';

	entry ("atime (access)", "%lu (%s)", st.st_atime, formatted_atime);

	if (! ctime_r ((time_t *)&st.st_mtime, formatted_mtime))
		die ("failed to format mtime");
	formatted_mtime[ strlen (formatted_mtime)-1] = '\0';

	entry ("mtime (modification)", "%lu (%s)", st.st_mtime, formatted_mtime);

	if (! ctime_r ((time_t *)&st.st_ctime, formatted_ctime))
		die ("failed to format ctime");
	formatted_ctime[ strlen (formatted_ctime)-1] = '\0';

	entry ("ctime (status change)", "%lu (%s)", st.st_ctime, formatted_ctime);

	section_close ();

	/*****************************************/

	footer ();
}

long
get_kernel_bits (void)
{
#if defined (PROCENV_LINUX) && ! defined (PROCENV_ANDROID)
	long value;

	errno = 0;
	value = sysconf (_SC_LONG_BIT);
	if (value == -1 && errno != 0)
		die ("failed to determine kernel bits");
	return value;
#endif
	return -1;
}

/* Dump out data in alphabetical fashion */
void
dump (void)
{
	master_header (&doc);

	show_meta ();
	show_arguments ();

#if defined (PROCENV_LINUX)
	show_capabilities ();
	show_linux_cgroups ();
#endif
	show_clocks ();
	show_compiler ();
#ifndef PROCENV_ANDROID
	show_confstrs ();
#endif
	show_env ();
	show_fds ();
#ifndef PROCENV_ANDROID
	show_libs ();
#endif
	show_rlimits ();
	show_locale ();
	show_misc ();
	show_msg_queues ();
	show_mounts (SHOW_ALL);
	show_network ();
#if defined (PROCENV_LINUX)
	show_oom ();
#endif
	show_platform ();
	show_proc ();
	show_ranges ();

	/* We should really call this last, to make figures as reliable
	 * as possible.
	 */
	show_rusage ();
	show_semaphores ();
	show_shared_mem ();
	show_signals ();
	show_sizeof ();
	show_stat ();
	show_sysconf ();
	show_threads ();
	show_time ();
	show_timezone ();
	show_tty_attrs ();
	show_uname ();

	master_footer (&doc);
}
void
get_network_address (const struct sockaddr *address, int family, char *name)
{
	int  ret;

	assert (address);
	assert (name);

	memset (name, '\0', NI_MAXHOST);

	if (family == AF_INET || family == AF_INET6) {
		ret = getnameinfo (address,
				(family == AF_INET)
				? sizeof (struct sockaddr_in)
				: sizeof (struct sockaddr_in6),
				(char *)name, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		if (ret) {
			switch (ret) {
			case EAI_NONAME:
			case EAI_FAMILY:
				sprintf ((char *)name, "%s", NA_STR);
				break;
			default:
				sprintf ((char *)name, "%s", UNKNOWN_STR);
				break;
			}
		}
	} else {
		sprintf ((char *)name, "%s", NA_STR);
	}

	assert (name[NI_MAXHOST-1] == '\0');
}

char *
decode_if_flags (unsigned int flags)
{
	char *str = NULL;
	struct if_flag_map *p;
	int first = TRUE;

	for (p = if_flag_map; p && p->name; p++) {
		if (flags & p->flag) {
			appendf (&str, "%s%s",
					first ? "" : ",",
					p->name);
			first = FALSE;
		}
	}

	return str;
}

const char *
get_ipv6_scope_name (uint32_t scope)
{
	switch (scope) {
		case 0x0:
		case 0xf:
			return "reserved";
			break;

		case 0x1:
			return "interface-local";
			break;

		case 0x2:
			return "link-local";
			break;

		case 0x4:
			return "admin-local";
			break;

		case 0x5:
			return "site-local";
			break;

		case 0x8:
			return "organization-local";
			break;

		case 0xe:
			return "global";
			break;
	}

	return UNKNOWN_STR;
}

int
get_mtu (const struct ifaddrs *ifaddr)
{
	int            sock;
	struct ifreq   ifr;
	int            request = SIOCGIFMTU;

	assert (ifaddr);

	/* We need to create a socket to query an interfaces mac
	 * address. Don't ask me why...
	 */
	sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (sock < 0)
		return -1;

	memset (&ifr, 0, sizeof (struct ifreq));
	strncpy (ifr.ifr_name, ifaddr->ifa_name, IFNAMSIZ-1);

	if (ioctl (sock, request, &ifr) < 0)
		goto out;
out:
	close (sock);

	return ifr.ifr_mtu;
}

/*
 *
 * Returns: IEEE-802 format MAC address, or NULL on error.
 */
char *
get_mac_address (const struct ifaddrs *ifaddr)
{
	char          *data = NULL;
	char          *mac_address = NULL;
	int            i;
	int            valid = FALSE;
#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	struct sockaddr_dl *link_layer;
#else
	struct ifreq   ifr;
	int            sock = -1;
#endif

#if defined (PROCENV_LINUX) || defined (PROCENV_HURD)
	int            request = SIOCGIFHWADDR;
#endif

	assert (ifaddr);

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	link_layer = (struct sockaddr_dl *)ifaddr->ifa_addr;
#else

	/* We need to create a socket to query an interfaces mac
	 * address. Don't ask me why...
	 */
	sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (sock < 0)
		return NULL;

	memset (&ifr, 0, sizeof (struct ifreq));
	strncpy (ifr.ifr_name, ifaddr->ifa_name, IFNAMSIZ-1);

	if (ioctl (sock, request, &ifr) < 0)
		goto out;
#endif

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	data = LLADDR (link_layer);
#else
	data = (char *)ifr.ifr_hwaddr.sa_data;
#endif

	if (data) {
		for (i = 0; i < 6; i++) {
			if (data[i]) {
				valid = TRUE;
				break;
			}
		}
	}

	/* MAC comprised of all zeros cannot be valid */
	if (! valid)
		goto out;

	/* An IEEE-802 formatted MAC address comprises 6x 2-byte groups,
	 * separated by 5 colons with an additional byte for the string
	 * terminator.
	 */
	mac_address = calloc ((6*2) + 5 + 1, sizeof (char));
	if (! mac_address)
		goto out;

	sprintf (mac_address, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
			(unsigned char)data[0],
			(unsigned char)data[1],
			(unsigned char)data[2],
			(unsigned char)data[3],
			(unsigned char)data[4],
			(unsigned char)data[5]);

out:

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	/* NOP */
#else
	close (sock);
#endif
	return mac_address;
}


#if defined (PROCENV_LINUX) || defined (PROCENV_HURD)
void
show_linux_mounts (ShowMountType what)
{
	FILE            *mtab;
	struct mntent   *mnt;
	struct statvfs   fs;
	unsigned int     major = 0;
	unsigned int     minor = 0;
	int              have_stats;
	char             canonical[PATH_MAX];

	common_assert ();

	mtab = fopen (MOUNTS, "r");

	if (! mtab)
		return;

	while ((mnt = getmntent (mtab))) {
		have_stats = TRUE;

		if (what == SHOW_ALL || what == SHOW_MOUNTS) {
			unsigned multiplier = 0;
			fsblkcnt_t blocks;
			fsblkcnt_t bfree;
			fsblkcnt_t bavail;
			fsblkcnt_t used_blocks;
			fsblkcnt_t used_files;

			if (statvfs (mnt->mnt_dir, &fs) < 0) {
				have_stats = FALSE;
			} else {
				multiplier = fs.f_bsize / DF_BLOCK_SIZE;

				blocks = fs.f_blocks * multiplier;
				bfree = fs.f_bfree * multiplier;
				bavail = fs.f_bavail * multiplier;
				used_blocks = blocks - bfree;
				used_files = fs.f_files - fs.f_ffree;
			}

			get_major_minor (mnt->mnt_dir,
					&major,
					&minor);

			section_open (mnt->mnt_dir);

			entry ("filesystem", "'%s'", mnt->mnt_fsname);

			get_canonical (mnt->mnt_fsname, canonical, sizeof (canonical));

			entry ("canonical", "'%s'", canonical);

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

				entry ("fsid", "%.*x", sizeof (fs.f_fsid), fs.f_fsid);
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
#endif

#if defined (PROCENV_LINUX)
char *
decode_extended_if_flags (const char *interface, unsigned short *flags)
{
	int                           sock;
	struct ifreq                  ifr;
	int                           first = TRUE;
	char                         *str = NULL;
	struct if_extended_flag_map  *p;

	assert (interface);
	assert (flags);

	/* We need to create a socket to query an interfaces mac
	 * address. Don't ask me why...
	 */
	sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (sock < 0)
		return NULL;

	memset (&ifr, 0, sizeof (struct ifreq));
	strncpy (ifr.ifr_name, interface, IFNAMSIZ-1);

	if (ioctl (sock, SIOCGIFPFLAGS, &ifr) < 0)
		goto out;

	*flags = ifr.ifr_flags;

	for (p = if_extended_flag_map; p && p->name; p++) {
		if (*flags & p->flag) {
			appendf (&str, "%s%s",
					first ? "" : ",",
					p->name);
			first = FALSE;
		}
	}
out:
	close (sock);
	return str;
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
 * Returns: TRUE if running Linux kernel is atleast at version
 * specified by (@major, @minor, @revision), else FALSE.
 **/
bool
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
		return TRUE;

	return FALSE;
}

#endif

void
show_mounts (ShowMountType what)
{
	common_assert ();

	header (what == SHOW_PATHCONF ? "pathconf" : "mounts");

#if defined (PROCENV_LINUX) || defined (PROCENV_HURD)
	show_linux_mounts (what);
#endif

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	show_bsd_mounts (what);
#endif

	footer ();
}

const char *
get_net_family_name (sa_family_t family)
{
	switch (family) {
#if defined (PROCENV_LINUX)
		case AF_PACKET:
			return "AF_PACKET";
			break;
#endif

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
		case AF_LINK:
			return "AF_LINK";
			break;
#endif

		case AF_INET:
			return "AF_INET";
			break;

		case AF_INET6:
			return "AF_INET6";
			break;
	}

	return UNKNOWN_STR;
}

void
show_network_if (const struct ifaddrs *ifa, const char *mac_address)
{
	char                *flags = NULL;
	char                *extended_flags = NULL;
	unsigned short       ext_flags = 0;
	sa_family_t          family;
	char                 address[NI_MAXHOST];
	int                  mtu = 0;

	common_assert ();
	assert (ifa);

	family = ifa->ifa_addr->sa_family;

	flags = decode_if_flags (ifa->ifa_flags);

#if defined (PROCENV_LINUX)
	extended_flags = decode_extended_if_flags (ifa->ifa_name, &ext_flags);
#endif

	section_open (ifa->ifa_name);

	entry ("family", "%s (0x%x)", get_net_family_name (family), family);
	entry ("flags", "0x%x (%s)", ifa->ifa_flags, flags ? flags : UNKNOWN_STR);
	entry ("extended flags", "0x%x (%s)", ext_flags, extended_flags ? extended_flags : NA_STR);

	if (flags)
		free (flags);

	if (extended_flags)
		free (extended_flags);

	mtu = get_mtu (ifa);

#if defined (PROCENV_HURD)
	/* No AF_LINK/AF_PACKET on Hurd atm */
	entry ("mac", "%s", UNKNOWN_STR);
#else
	entry ("mac", "%s", mac_address ? mac_address : NA_STR);
#endif

	if (mtu > 0) {
		entry ("mtu", "%d", mtu);
	} else {
		entry ("mtu", "%s", UNKNOWN_STR);
	}

	get_network_address (ifa->ifa_addr, family, address);
	entry ("address", "%s", address);

	if (ifa->ifa_netmask)
		get_network_address (ifa->ifa_netmask, family, address);
	entry ("netmask", "%s", ifa->ifa_netmask ? address : NA_STR);

#if !defined (PROCENV_HURD)
	if (family != PROCENV_LINK_LEVEL_FAMILY) {
		if ((ifa->ifa_flags & IFF_BROADCAST) && ifa->ifa_broadaddr) {
			get_network_address (ifa->ifa_broadaddr, family, address);

			entry ("broadcast", "%s", ifa->ifa_broadaddr ? address : NA_STR);
		}
	} else {
#endif
		entry ("broadcast", "%s", NA_STR);
#if !defined (PROCENV_HURD)
	}
#endif

	if (ifa->ifa_flags & IFF_POINTOPOINT && ifa->ifa_dstaddr) {

		get_network_address (ifa->ifa_dstaddr, family, address);

		entry ("point-to-point", "%s", address);
	}

	section_close ();
}

/*
 * BSD returns an additional AF_LINK ifaddrs containing the
 * actual link-layer MAC address for each interface.
 *
 * Linux does the same but with one additional AF_PACKET family / interface.
 *
 * This is somewhat noisome since for BSD we need to cache the MAC addresses
 * such that they can be retrieved when the _next_ ifaddrs structure
 * appears for the *same* interface, but we only want to
 * display the non-AF_LINK elements *unless* they refer
 * to an interface with no associated address.
 *
 * The situation for Linux is similar but we only care
 * about AF_PACKET entries for interfaces that have no
 * associated address since we can extract the MAC
 * address using an ioctl (rather than considering the
 * AF_PACKET element).
 *
 * The strategy therefore is to:
 *
 * 1) Cache all AF_LINK/AF_PACKET elements.
 * 2) Once an element arrives that matches an interface
 *    name found in the cache, use that as necessary (extra
 *    the MAC for BSD, NOP for Linux), then free that cache
 *    element.
 * 3) Having processed all entries, if any entries are
 * left in the cache, they must refer to interfaces that
 * have no address, so display them.
 *
 * XXX: Note the implicit assumption that the AF_LINK/AF_PACKET entries
 * will appear _before_ the corresponding entry containing address
 * details for the interface in the output of getifaddrs(): observations
 * suggests this _seems_ to be the case, but is not documented as being
 * guaranteed.
 */
#ifdef PROCENV_ANDROID

void
show_network (void)
{
	header ("network");
	/* Bionic isn't actually that bionic at all :( */
	show ("%s", UNKNOWN_STR);
}

#else

void
show_network (void)
{
	struct ifaddrs      *if_addrs;
	struct ifaddrs      *ifa;
	char                *mac_address = NULL;
	struct network_map  *head = NULL;
	struct network_map  *node = NULL;
	struct network_map  *tmp = NULL;

	common_assert ();

	header ("network");

	/* Query all network interfaces */
	if (getifaddrs (&if_addrs) < 0)
		return;

	/* Construct an initial node for the cache */
	head = calloc (1, sizeof (struct network_map));
	assert (head);

	/* Iterate over all network interfaces */
	for (ifa = if_addrs; ifa; ifa = ifa->ifa_next) {
#if !defined (PROCENV_HURD)
		int family;
#endif

		if (! ifa->ifa_addr)
			continue;

#if !defined (PROCENV_HURD)
		family = ifa->ifa_addr->sa_family;

		if (family == PROCENV_LINK_LEVEL_FAMILY) {

			/* Add link level interface details to the cache */
			mac_address = get_mac_address (ifa);

			node = calloc (1, sizeof (struct network_map));
			assert (node);

			/* Conveniently, an ifaddrs contains a bunch of
			 * pointers and some flags.
			 *
			 * Since all those pointers are valid until we
			 * call freeifaddrs(), all we need to do is copy
			 * the flags since the memcpy will copy the
			 * pointers addresses for us :)
			 */
			memcpy (&node->ifaddr, ifa, sizeof (struct ifaddrs));
			node->ifaddr.ifa_flags = ifa->ifa_flags;

			/* Since we've already formatted the MAC
			 * address, we'll cache that too.
			 */
			node->mac_address = mac_address;
			mac_address = NULL;

			/* prepend */
			node->next = head->next;
			if (head->next)
				head->next->prev = node;
			node->prev = head;
			head->next = node;

			continue;
		}
#endif

		/* From now on, we're only looking at interfaces with an
		 * address.
		 */

		/* Search for an entry corresponding to the interface in the cache */
		for (node = head->next; node && node->ifaddr.ifa_name; node = node->next) {
			if (! strcmp (node->ifaddr.ifa_name, ifa->ifa_name)) {

				/* Save */
				mac_address = node->mac_address
					? strdup (node->mac_address)
					: NULL;

				/* Unlink existing node as it has now served its purpose */
				node->prev->next = node->next;
				if (node->next)
					node->next->prev = node->prev;

				/* Destroy */
				if (node->mac_address)
					free (node->mac_address);
				free (node);

				break;
			}
		}

		/* Display the interface (which must have an associated address) */
		show_network_if (ifa, mac_address);
		if (mac_address) {
			free (mac_address);
			mac_address = NULL;
		}
	}

	/* Destroy the cache, displaying any interfaces not previously displayed.
	 * These by definition cannot have addresses assigned to them.
	 */
	for (node = head->next; node && node->ifaddr.ifa_name; node = tmp) {

		tmp = node->next;

		show_network_if (&node->ifaddr, node->mac_address);

		/* Destroy */
		if (node->mac_address)
			free (node->mac_address);
		free (node);
	}

	free (head);
	freeifaddrs (if_addrs);

	footer ();
}
#endif

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)

char *
get_bsd_mount_opts (uint64_t flags)
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

void
show_bsd_mounts (ShowMountType what)
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
	int               ret;

	common_assert ();

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
			char *str = NULL;

			ret = get_major_minor (mnt->f_mntonname,
					&major,
					&minor);

			multiplier = mnt->f_bsize / DF_BLOCK_SIZE;
			blocks = mnt->f_blocks * multiplier;
			bfree = mnt->f_bfree * multiplier;
			bavail = mnt->f_bavail * multiplier;
			used = blocks - bfree;

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
				sizeof (mnt->f_fsid.val[0]),
				mnt->f_fsid.val[0],
				sizeof (mnt->f_fsid.val[1]),
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

/* FIXME */
void
show_shared_mem_bsd (void)
{
	header ("shared memory");

	show ("%s", NOT_IMPLEMENTED_STR);
}

/* FIXME */
void
show_semaphores_bsd (void)
{
	header ("semaphores");

	show ("%s", NOT_IMPLEMENTED_STR);
}

/* FIXME */
void
show_msg_queues_bsd (void)
{
	header ("message queues");

	show ("%s", NOT_IMPLEMENTED_STR);
}

#endif

void
get_priorities (void)
{
	priority.process = getpriority (PRIO_PROCESS, 0);
	priority.pgrp    = getpriority (PRIO_PGRP   , 0);
	priority.user    = getpriority (PRIO_USER   , 0);
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
	char         buffer[1024];
	FILE        *f;
#if defined (PROCENV_LINUX)
	dev_t        expected;

	expected = makedev (5, 1);
#endif

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

	/* compiler appeasement */
	return FALSE;
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
	common_assert ();

#if defined (PROCENV_LINUX)
	show_linux_proc_branch ();
#endif

#if defined (PROCENV_BSD)
	show_bsd_proc_branch ();
#endif
}

#if defined (PROCENV_BSD)
/* Who would have thought handling PIDs was so tricky? */
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
					int                  ppid_found = FALSE;
					int                  j;

					/* Determine if the parent PID
					 * actually exists within the
					 * jail.
					 */
					for (j = 0; j < count; j++) {
						p2 =  &procs[j];

						if (p2->ki_pid == p->ki_ppid) {
							ppid_found = TRUE;
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
						done = TRUE;
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
					done = TRUE;
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
#endif

#if defined (PROCENV_LINUX)
void
show_linux_prctl (void)
{
	int  rc;
	int  arg2;
	char name[17] = { 0 };

	common_assert ();

	section_open ("prctl");

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

#if defined (PR_GET_TSC) && defined (PROCENV_ARCH_X86)
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

	section_close ();
}

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
	char   *str = NULL;

	common_assert ();

	sprintf (pid, "%d", (int)getpid ());

	/* This is one God-awful interface */
	while (TRUE) {
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
#endif

/* More verbose version of stty(1).
 *
 * Tries to obtain a tty fd by considering the standard streams if the
 * default fails.
 **/
void
show_tty_attrs (void)
{
	struct termios  tty;
	struct termios  lock_status;
	struct winsize  size;
	int             ret;
	int             fds[4] = { -1, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO };
	size_t          i;

	common_assert ();

	fds[0] = user.tty_fd;

	/* For non-Linux platforms, this will force the lock
	 * status to be unlocked.
	 */
	memset (&lock_status, '\0', sizeof (lock_status));

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

	/* cannot query attributes */
	footer ();
	return;

work:
	user.tty_fd = fds[i];

#ifdef PROCENV_LINUX
	get_tty_locked_status (&lock_status);
#endif

	/*****************************************/
	section_open ("c_iflag (input)");

	entry ("value", "0x%x", tty.c_iflag);

	show_const_tty (tty, c_iflag, IGNBRK, lock_status);
	show_const_tty (tty, c_iflag, BRKINT, lock_status);
	show_const_tty (tty, c_iflag, IGNPAR, lock_status);
	show_const_tty (tty, c_iflag, PARMRK, lock_status);
	show_const_tty (tty, c_iflag, INPCK, lock_status);
	show_const_tty (tty, c_iflag, ISTRIP, lock_status);
	show_const_tty (tty, c_iflag, INLCR, lock_status);
	show_const_tty (tty, c_iflag, IGNCR, lock_status);
	show_const_tty (tty, c_iflag, ICRNL, lock_status);
#if defined (PROCENV_LINUX)
	show_const_tty (tty, c_iflag, IUCLC, lock_status);
#endif
	show_const_tty (tty, c_iflag, IXON, lock_status);
	show_const_tty (tty, c_iflag, IXANY, lock_status);
	show_const_tty (tty, c_iflag, IXOFF, lock_status);
	show_const_tty (tty, c_iflag, IMAXBEL, lock_status);
#if defined (PROCENV_LINUX)
	show_const_tty (tty, c_iflag, IUTF8, lock_status);
#endif

	section_close ();

	/*****************************************/
	section_open ("c_oflag (output)");

	entry ("c_oflag", "0x%x", tty.c_oflag);

	show_const_tty (tty, c_oflag, OPOST, lock_status);
#if defined (PROCENV_LINUX)
	show_const_tty (tty, c_oflag, OLCUC, lock_status);
#endif
	show_const_tty (tty, c_oflag, ONLCR, lock_status);
	show_const_tty (tty, c_oflag, OCRNL, lock_status);
	show_const_tty (tty, c_oflag, ONOCR, lock_status);
	show_const_tty (tty, c_oflag, ONLRET, lock_status);
#if defined (PROCENV_LINUX)
	show_const_tty (tty, c_oflag, OFILL, lock_status);
	show_const_tty (tty, c_oflag, OFDEL, lock_status);
	show_const_tty (tty, c_oflag, NLDLY, lock_status);
	show_const_tty (tty, c_oflag, CRDLY, lock_status);
#endif
	show_const_tty (tty, c_oflag, TABDLY, lock_status);
#if defined (PROCENV_LINUX)
	show_const_tty (tty, c_oflag, BSDLY, lock_status);
	show_const_tty (tty, c_oflag, VTDLY, lock_status);
	show_const_tty (tty, c_oflag, FFDLY, lock_status);
#endif

	section_close ();

	/*****************************************/
	section_open ("c_cflag (control)");

	entry ("value", "0x%x", tty.c_cflag);

#if defined (PROCENV_LINUX)
	show_const_tty (tty, c_cflag, CBAUDEX, lock_status);
#endif
	show_const_tty (tty, c_cflag, CSIZE, lock_status);
	show_const_tty (tty, c_cflag, CSTOPB, lock_status);
	show_const_tty (tty, c_cflag, CREAD, lock_status);
	show_const_tty (tty, c_cflag, PARENB, lock_status);
	show_const_tty (tty, c_cflag, PARODD, lock_status);
	show_const_tty (tty, c_cflag, HUPCL, lock_status);
	show_const_tty (tty, c_cflag, CLOCAL, lock_status);
#if defined (PROCENV_LINUX)
#ifdef CIBAUD
	show_const_tty (tty, c_cflag, CIBAUD, lock_status);
#endif
#ifdef CMSPAR
	show_const_tty (tty, c_cflag, CMSPAR, lock_status);
#endif
#endif
	show_const_tty (tty, c_cflag, CRTSCTS, lock_status);

	section_close ();

	/*****************************************/
	section_open ("c_lflag (local)");

	entry ("value", "0x%x", tty.c_lflag);

	show_const_tty (tty, c_lflag, ISIG, lock_status);
#if defined (PROCENV_LINUX)
	show_const_tty (tty, c_lflag, XCASE, lock_status);
#endif
	show_const_tty (tty, c_lflag, ICANON, lock_status);
	show_const_tty (tty, c_lflag, ECHO, lock_status);
	show_const_tty (tty, c_lflag, ECHOE, lock_status);
	show_const_tty (tty, c_lflag, ECHOK, lock_status);
	show_const_tty (tty, c_lflag, ECHONL, lock_status);
	show_const_tty (tty, c_lflag, ECHOCTL, lock_status);
	show_const_tty (tty, c_lflag, ECHOPRT, lock_status);
	show_const_tty (tty, c_lflag, ECHOKE, lock_status);
	show_const_tty (tty, c_lflag, FLUSHO, lock_status);
	show_const_tty (tty, c_lflag, NOFLSH, lock_status);
	show_const_tty (tty, c_lflag, TOSTOP, lock_status);
	show_const_tty (tty, c_lflag, PENDIN, lock_status);
	show_const_tty (tty, c_lflag, IEXTEN, lock_status);

	section_close ();

	/*****************************************/
	section_open ("c_cc (special)");

	show_cc_tty (tty, VINTR, lock_status);
	show_cc_tty (tty, VQUIT, lock_status);
	show_cc_tty (tty, VERASE, lock_status);
	show_cc_tty (tty, VKILL, lock_status);
	show_cc_tty (tty, VEOF, lock_status);
	show_cc_tty (tty, VTIME, lock_status);
	show_cc_tty (tty, VMIN, lock_status);
#if defined (PROCENV_LINUX)
	show_cc_tty (tty, VSWTC, lock_status);
#endif
	show_cc_tty (tty, VSTART, lock_status);
	show_cc_tty (tty, VSTOP, lock_status);
	show_cc_tty (tty, VSUSP, lock_status);
	show_cc_tty (tty, VEOL, lock_status);
	show_cc_tty (tty, VREPRINT, lock_status);
	show_cc_tty (tty, VDISCARD, lock_status);
	show_cc_tty (tty, VWERASE, lock_status);
	show_cc_tty (tty, VLNEXT, lock_status);
	show_cc_tty (tty, VEOL2, lock_status);

	section_close ();

	if (ioctl (user.tty_fd, TIOCGWINSZ, &size) < 0)
		die ("failed to determine terminal dimensions");

	/*****************************************/
	section_open ("speed");

	entry ("input (baud)", "%s",
			get_speed (cfgetispeed (&tty)));

	entry ("output (baud)", "%s",
			get_speed (cfgetospeed (&tty)));

	section_close ();

	/*****************************************/
	section_open ("winsize");

	entry ("ws_row", "%u", size.ws_row);
	entry ("ws_col", "%u", size.ws_col);
	entry ("ws_xpixel", "%u", size.ws_xpixel);
	entry ("ws_ypixel", "%u", size.ws_ypixel);

	section_close ();

	/*****************************************/

	footer ();
}

void
show_locale (void)
{
	struct procenv_map *p;
	char               *value;
	char               *v;
	char               *saved = NULL;

	common_assert ();

	header ("locale");

	v = getenv ("LANG");
	entry ("LANG", "%s", v ? v : "");

	v = getenv ("LANGUAGE");
	entry ("LANGUAGE", "%s", v ? v : "");

	value = setlocale (LC_ALL, "");
	if (value) {
		saved = strdup (value);
		if (! saved)
			die ("failed to allocate space for locale");
	}

	for (p = locale_map; p && p->name; p++) {
		value = setlocale (p->num, NULL);
		entry (p->name, "%s", value ? value : UNKNOWN_STR);
	}

	v = getenv ("LC_ALL");
	entry ("LC_ALL", "%s", v ? v : "");

	if (saved) {
		(void)setlocale (LC_ALL, saved);
		free (saved);
	}

    footer ();
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

/**
 * get_os:
 *
 * Returns: static string representing best guess
 * at operating system.
 **/
char *
get_os (void)
{
#ifdef _AIX
	return "AIX";
#endif

#ifdef __ANDROID__
	return "Android";
#endif

#ifdef __FreeBSD__
	return "FreeBSD";
#endif

#if defined (__MACH__) || defined (__GNU__) || defined (__gnu_hurd__)
	return "GNU (Hurd)";
#endif

#if defined (__hpux) || defined (hpux) || defined (_hpux)
	return "HP-UX";
#endif

#if defined (OS400) || defined (__OS400__)
	return "iSeries (OS/400)";
#endif

#if defined (__FreeBSD_kernel__) && defined (__GNUC__)
	return "GNU/kFreeBSD";
#endif

#ifdef PROCENV_LINUX
#ifdef __s390x__
	return "Linux (zSeries)";
#endif
#ifdef __s390__
	return "Linux (S/390)";
#endif

#ifdef __sh__
	return "Linux (SuperH)";
#endif

	return "Linux";
#endif

#ifdef _NetBSD__
	return "NetBSD";
#endif

#ifdef _OpenBSD__
	return "OpenBSD";
#endif

#ifdef VMS
	return "OpenVMS";
#endif

#if defined (sun) || defined (__sun)
	return "Solaris";
#endif

#ifdef __osf__
	return "Tru64";
#endif

#ifdef WINDOWS
	return "Windows";
#endif

#ifdef __MVS__
	return "z/OS (MVS)";
#endif

	return UNKNOWN_STR;
}

/**
 * get_arch:
 *
 * Returns: static string representing best guess
 * at architecture.
 **/
char *
get_arch (void)
{

#ifdef __arm__
#ifdef __aarch64__
	return "ARM64";
#endif
#ifdef __ARM_PCS_VFP
	return "ARMhf";
#endif
#ifdef __ARMEL__
	return "ARMEL";
#endif
	return "ARM";
#endif

#ifdef __hppa__
	return "HP/PA RISC";
#endif

#ifdef __i386__
	return "i386";
#endif

#ifdef __ia64__
	return "IA64";
#endif

#ifdef __MIPSEL__
	return "MIPSEL";
#endif

#ifdef __mips__
	return "MIPS";
#endif

#ifdef __powerpc__
	return "PowerPC";
#endif

#ifdef __sparc64__
	return "Sparc64";
#endif

#ifdef __sparc__
	return "Sparc";
#endif

#ifdef __alpha__
	return "Alpha";
#endif
#ifdef __m68k__
	return "m68k";
#endif

#ifdef __ILP32__
	if (sizeof (void *) == 4)
		return "x32";
#endif

#if defined (__s390__) || defined (__zarch__) || defined (__SYSC_ZARCH__) || defined (__THW_370__)
	return "SystemZ";
#endif

#if defined (__x86_64__) || defined (__x86_64) || defined (__amd64)
	return "x64/AMD64";
#endif

	return UNKNOWN_STR;
}

#ifndef PROCENV_ANDROID
int
libs_callback (struct dl_phdr_info *info, size_t size, void *data)
{
	const char *name;
	const char *path;

	assert (info);

	if (! info->dlpi_name || ! *info->dlpi_name)
		return 0;

	path = info->dlpi_name;
	name = strrchr (path, '/');

	if (name) {
		/* Jump over slash */
		name++;
	} else {
		/* BSD libraries don't show the path */
		name = path;
	}

	object_open (FALSE);

	container_open (name);

	entry ("path", "%s", path);
	entry ("address", "%p", (void *)info->dlpi_addr);

	container_close ();

	object_close (FALSE);

	return 0;
}

void
show_libs (void)
{
	common_assert ();

	container_open ("libraries");

	dl_iterate_phdr (libs_callback, NULL);

	container_close ();
}
#endif

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

#if defined (PROCENV_LINUX) || defined (PROCENV_HURD)
#ifdef CLOCK_MONOTONIC_RAW
	show_clock_res (CLOCK_MONOTONIC_RAW);
#endif
	show_clock_res (CLOCK_PROCESS_CPUTIME_ID);
	show_clock_res (CLOCK_THREAD_CPUTIME_ID);
#endif

    footer ();
}

void
show_timezone (void)
{
#if defined (PROCENV_LINUX)
	tzset ();

	header ("timezone");

	entry ("tzname[0]", "'%s'", tzname[0]);
	entry ("tzname[1]", "'%s'", tzname[1]);
	entry ("timezone", "%ld", timezone);
	entry ("daylight", "%d", daylight);

    footer ();
#endif
}

void
show_sizeof (void)
{
	header ("sizeof");

	entry ("bits/byte (CHAR_BIT)", "%d", CHAR_BIT);

	/* fundamental types and non-aggregate typedefs */

	show_sizeof_type (char);
	show_sizeof_type (short int);
	show_sizeof_type (int);

	show_sizeof_type (long int);

	show_sizeof_type (long long int);

	show_sizeof_type (float);

	show_sizeof_type (double);

	show_sizeof_type (long double);

	show_sizeof_type (size_t);
	show_sizeof_type (ssize_t);
	show_sizeof_type (ptrdiff_t);
	show_sizeof_type (void *);
	show_sizeof_type (wchar_t);

	show_sizeof_type (intmax_t);
	show_sizeof_type (uintmax_t);
	show_sizeof_type (imaxdiv_t);
	show_sizeof_type (intptr_t);
	show_sizeof_type (uintptr_t);

	show_sizeof_type (time_t);
	show_sizeof_type (clock_t);

	show_sizeof_type (sig_atomic_t);
	show_sizeof_type (off_t);
	show_sizeof_type (fpos_t);
	show_sizeof_type (mode_t);

	show_sizeof_type (pid_t);
	show_sizeof_type (uid_t);
	show_sizeof_type (gid_t);

	show_sizeof_type (rlim_t);
	show_sizeof_type (fenv_t);
	show_sizeof_type (fexcept_t);

	show_sizeof_type (wint_t);
	show_sizeof_type (div_t);
	show_sizeof_type (ldiv_t);
	show_sizeof_type (lldiv_t);
	show_sizeof_type (mbstate_t);

	footer ();
}

void
show_ranges (void)
{
	header ("ranges");

	/******************************/
	section_open ("char");

	show_size (char);

	section_open ("unsigned");

	entry ("decimal", "%u to %u", 0, UCHAR_MAX);

	entry ("scientific", "%e to %e", (double)0, (double)UCHAR_MAX);
	entry ("hex", "0x%.*x to 0x%.*x",
			type_hex_width (char), 0,
			type_hex_width (char), UCHAR_MAX);

	section_close ();

	section_open ("signed");
	entry ("decimal", "%d to %d", CHAR_MIN, CHAR_MAX);
	section_close ();

	section_close ();

	/******************************/
	section_open ("short int");

	show_size (short int);
	section_open ("unsigned");
	entry ("decimal", "%u to %u", 0, USHRT_MAX);
	entry ("scientific", "%e to %e", (double)0, (double)USHRT_MAX);
	entry ("hex", "0x%.*x to 0x%.*x",
			type_hex_width (short int), 0,
			type_hex_width (short int), USHRT_MAX);
	section_close ();

	section_open ("signed");
	entry ("decimal", "%d to %d", SHRT_MIN, SHRT_MAX);
	section_close ();

	section_close ();

	/******************************/
	section_open ("int");

	show_size (int);
	section_open ("unsigned");
	entry ("decimal", "%u to %u", 0, UINT_MAX);
	entry ("scientific", "%e to %e", (double)0, (double)UINT_MAX);
	entry ("hex", "0x%.*x to 0x%.*x",
			type_hex_width (int), 0,
			type_hex_width (int), UINT_MAX);
	section_close ();

	section_open ("signed");
	entry ("decimal", "%d to %d", INT_MIN, INT_MAX);
	section_close ();

	section_close ();

	/******************************/
	section_open ("long int");

	show_size (long int);
	section_open ("unsigned");
	entry ("decimal", "%u to %u", 0, ULONG_MAX);
	entry ("scientific", "%e to %e", (double)0, (double)ULONG_MAX);
	entry ("hex", "0x%.*x to 0x%.*x",
			type_hex_width (long int), 0L,
			type_hex_width (long int), ULONG_MAX);
	section_close ();

	section_open ("signed");
	entry ("decimal", "%ld to %ld", LONG_MIN, LONG_MAX);
	section_close ();

	section_close ();

	/******************************/
	section_open ("long long int");

	show_size (long long int);
	section_open ("unsigned");
	entry ("decimal", "%llu to %llu", 0, ULLONG_MAX);
	entry ("scientific", "%e to %e", (double)0, (double)ULLONG_MAX);
	entry ("hex", "0x%.*llx to 0x%.*llx",
			type_hex_width (long long int), 0LL,
			type_hex_width (long long int), ULLONG_MAX);
	section_close ();

	section_open ("signed");
	entry ("decimal", "%lld to %lld", LLONG_MIN, LLONG_MAX);
	section_close ();

	section_close ();

	/******************************/
	section_open ("float");
	show_size (float);
	entry ("signed", "%e to %e", FLT_MIN, FLT_MAX);
	section_close ();

	/******************************/
	section_open ("double");
	show_size (double);
	entry ("signed", "%le to %le", DBL_MIN, DBL_MAX);
	section_close ();

	/******************************/
	section_open ("long double");
	show_size (long double);
	entry ("signed", "%Le to %Le", LDBL_MIN, LDBL_MAX);
	section_close ();

	/******************************/

    footer ();
}

void
show_compiler (void)
{
	char *name = NULL;
	char *version = NULL;

#if defined (__INTEL_COMPILER)
	name = "Intel";
	version = __ICC;
#elif defined (__clang__)
	name = "Clang/LLVM";
	version = __clang_version__;
#elif defined (__GNUC__)
	name = "GCC";
	version = __VERSION__;
#endif

	header ("compiler");

	entry ("name", "%s", name ? name : UNKNOWN_STR);
	entry ("version", "%s", version ? version : UNKNOWN_STR);
	entry ("compile date (__DATE__)", "%s", __DATE__);
	entry ("compile time (__TIME__)", "%s", __TIME__);
	entry ("translation unit (__FILE__)", "%s", __FILE__);
	entry ("base file (__BASE_FILE__)", "%s", __BASE_FILE__);
	entry ("timestamp (__TIMESTAMP__)", "%s", __TIMESTAMP__);

#ifdef __STRICT_ANSI__
	entry ("__STRICT_ANSI__", "%s", DEFINED_STR);
#else
	entry ("__STRICT_ANSI__", "%s", NOT_DEFINED_STR);
#endif

#ifdef _POSIX_C_SOURCE
	entry ("_POSIX_C_SOURCE", "%lu", _POSIX_C_SOURCE);
#else
	entry ("_POSIX_C_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _POSIX_SOURCE
	entry ("_POSIX_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_POSIX_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _XOPEN_SOURCE
	entry ("_XOPEN_SOURCE", "%lu", _XOPEN_SOURCE);
#else
	entry ("_XOPEN_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _XOPEN_SOURCE_EXTENDED
	entry ("_XOPEN_SOURCE_EXTENDED", "%s", DEFINED_STR);
#else
	entry ("_XOPEN_SOURCE_EXTENDED", "%s", NOT_DEFINED_STR);
#endif

#ifdef _ISOC95_SOURCE
	entry ("_ISOC95_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_ISOC95_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _ISOC99_SOURCE
	entry ("_ISOC99_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_ISOC99_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _ISOC11_SOURCE
	entry ("_ISOC11_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_ISOC11_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _LARGEFILE64_SOURCE
	entry ("_LARGEFILE64_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_LARGEFILE64_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _FILE_OFFSET_BITS
	entry ("_FILE_OFFSET_BITS", "%lu", _FILE_OFFSET_BITS);
#else
	entry ("_FILE_OFFSET_BITS", "%s", NOT_DEFINED_STR);
#endif

#ifdef _BSD_SOURCE
	entry ("_BSD_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_BSD_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _SVID_SOURCE
	entry ("_SVID_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_SVID_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _ATFILE_SOURCE
	entry ("_ATFILE_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_ATFILE_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _GNU_SOURCE
	entry ("_GNU_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_GNU_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _REENTRANT
	entry ("_REENTRANT", "%s", DEFINED_STR);
#else
	entry ("_REENTRANT", "%s", NOT_DEFINED_STR);
#endif

#ifdef _THREAD_SAFE
	entry ("_THREAD_SAFE", "%s", DEFINED_STR);
#else
	entry ("_THREAD_SAFE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _FORTIFY_SOURCE
	entry ("_FORTIFY_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_FORTIFY_SOURCE", "%s", NOT_DEFINED_STR);
#endif

    footer ();
}

void
show_time (void)
{
	char              formatted_time[CTIME_BUFFER];
	struct timespec   ts;
	struct tm        *tm;

	if (clock_gettime (CLOCK_REALTIME, &ts) < 0)
		die ("failed to determine time");

	tm = localtime (&ts.tv_sec);
	if (! tm)
		die ("failed to determine localtime");

	if (! asctime_r (tm, formatted_time))
		die ("failed to determine formatted time");

	/* overwrite trailing '\n' */
	formatted_time[strlen (formatted_time)-1] = '\0';

	header ("time");

	entry ("raw", "%u.%lu",
			(unsigned int)ts.tv_sec,
			ts.tv_nsec);

	entry ("local", "%s", formatted_time);

	entry ("ISO", "%4.4d-%2.2d-%2.2dT%2.2d:%2.2d",
			1900+tm->tm_year,
			tm->tm_mon,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min);

    footer ();
}

void
get_uname (void)
{
	if (uname (&uts) < 0)
		die ("failed to query uname");
}

void
show_uname (void)
{
	header ("uname");

	entry ("sysname", "%s", uts.sysname);
	entry ("nodename", "%s", uts.nodename);
	entry ("release", "%s", uts.release);
	entry ("version", "%s", uts.version);
	entry ("machine", "%s", uts.machine);

#if defined (_GNU_SOURCE) && defined (PROCENV_LINUX)
	entry ("domainname", "%s", uts.domainname);
#endif

    footer ();
}

#if defined (PROCENV_LINUX)

void
show_capabilities (void)
{
	int ret;

	header ("capabilities(linux)");

	container_open ("list");

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

	if (LINUX_KERNEL_MM (2, 4)) {
		show_capability (CAP_MKNOD);
		show_capability (CAP_LEASE);
	}
	if (LINUX_KERNEL_MMR (2, 6, 11)) {
		show_capability (CAP_AUDIT_WRITE);
		show_capability (CAP_AUDIT_CONTROL);
	}
	if (LINUX_KERNEL_MMR (2, 6, 24))
		show_capability (CAP_SETFCAP);
	if (LINUX_KERNEL_MMR (2, 6, 25)) {
		show_capability (CAP_MAC_OVERRIDE);
		show_capability (CAP_MAC_ADMIN);
	}

#ifdef CAP_SYSLOG
	if (LINUX_KERNEL_MMR (2, 6, 37))
		show_capability (CAP_SYSLOG);

#endif

#ifdef CAP_WAKE_ALARM
	if (LINUX_KERNEL_MM (3, 0))
		show_capability (CAP_WAKE_ALARM);
#endif

	container_close ();

#ifdef PR_GET_KEEPCAPS
	if (LINUX_KERNEL_MMR (2, 2, 18)) {
		ret = prctl (PR_GET_KEEPCAPS, 0, 0, 0, 0);
		if (ret < 0)
			entry ("keep", "%s", UNKNOWN_STR);
		else
			entry ("keep", "%s", ret ? YES_STR : NO_STR);
	}
#endif


#if defined (PR_GET_SECUREBITS) && defined (HAVE_LINUX_SECUREBITS_H)
	if (LINUX_KERNEL_MMR (2, 6, 26)) {
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

	footer ();
}

void
show_linux_security_module (void)
{
	char *lsm = UNKNOWN_STR;
#if defined (HAVE_APPARMOR)
	if (aa_is_enabled ())
		lsm = "AppArmor";
#endif
#if defined (HAVE_SELINUX)
	if (is_selinux_enabled ())
		lsm = "SELinux";
#endif
	entry ("Linux Security Module", "%s", lsm);
}

void
show_linux_security_module_context (void)
{
	char   *context = NULL;
	char   *mode = NULL;

#if defined (HAVE_APPARMOR)
	if (aa_is_enabled ())
		if (aa_gettaskcon (user.pid, &context, &mode) < 0)
			die ("failed to query AppArmor context");
#endif
#if defined (HAVE_SELINUX)
	if (is_selinux_enabled ())
		if (getpidcon (user.pid, &context) < 0)
			die ("failed to query SELinux context");
#endif
	if (context) {
		if (mode)
			entry ("LSM context", "%s (%s)", context, mode);
		else
			entry ("LSM context", "%s", context);
	} else
		entry ("LSM context", "%s", UNKNOWN_STR);

	free (context);
	free (mode);
}

void
show_linux_cgroups (void)
{
	const  char  *delim = ":";
	char         *file = "/proc/self/cgroup";
	FILE         *f;
	char          buffer[1024];
	size_t        len;

	header ("cgroups(linux)");

	f = fopen (file, "r");

	if (! f)
		goto out;

	while (fgets (buffer, sizeof (buffer), f)) {
		char  *buf;
		char  *hierarchy;
		char  *subsystems;
		char  *path;

		len = strlen (buffer);
		/* Remove NL */
		buffer[len-1] = '\0';

		buf = strdup (buffer);
		if (! buf)
			die ("failed to alloate storage");

		hierarchy = strsep (&buf, delim);
		if (! hierarchy)
			goto next;

		subsystems = strsep (&buf, delim);
		if (! subsystems)
			goto next;

		path = strsep (&buf, delim);
		if (! path)
			goto next;

		/* FIXME: should sort by hierarchy */
		container_open (hierarchy);

		/* FIXME: should split this on comma */
		entry ("subsystems", "%s", subsystems);

		entry ("path", "%s", path);

		container_close ();

next:
		free (buf);
	}

	fclose (f);

out:

	footer ();
}

void
show_fds_linux (void)
{
	DIR            *dir;
	struct dirent  *ent;
	char           *prefix_path = "/proc/self/fd";
	struct stat     st;
	char            path[MAXPATHLEN];
	char            link[MAXPATHLEN];
	ssize_t         len;
	int             container_disabled = FALSE;

	container_open ("file descriptors");

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

			if (stat (link, &st) < 0)
				continue;

			/* Ignore the last (invalid) entry */
			if (S_ISDIR (st.st_mode))
				continue;
		}

		if (output_format == OUTPUT_FORMAT_JSON &&
				in_container == TRUE) {
			/* Nasty hack to produce valid JSON */
			container_disabled = TRUE;
			in_container = FALSE;
			object_open (FALSE);
		}

		section_open (num);

		entry ("terminal", "%s", isatty (fd) ? YES_STR : NO_STR);
		entry ("valid", "%s", fd_valid (fd) ? YES_STR : NO_STR);
		entry ("device", "%s", link);

		section_close ();

		if (output_format == OUTPUT_FORMAT_JSON &&
				container_disabled == TRUE) {
			object_close (FALSE);

			/* re-enable */
			in_container = TRUE;
		}

		free (num);
	}

	closedir (dir);

	container_close ();
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
	int      seen = FALSE;

	header ("oom(linux)");

	for (file = files; file && *file; file++) {
		ret = sprintf (path, "%s/%s", dir, *file);
		if (ret < 0)
			continue;

		f = fopen (path, "r");
		if (! f)
			continue;

		seen = TRUE;

		while (fgets (buffer, sizeof (buffer), f)) {
			len = strlen (buffer);
			buffer[len-1] = '\0';
			entry (*file, "%s", buffer);
		}

		fclose (f);
	}

	if (! seen)
		entry ("%s", UNKNOWN_STR);

	footer ();
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
show_linux_cpu (void)
{
	int cpu;
	long max;

	max = get_sysconf (_SC_NPROCESSORS_ONLN);

#if HAVE_SCHED_GETCPU
	cpu = sched_getcpu ();
	if (cpu < 0)
		goto unknown_sched_cpu;

	/* adjust to make 1-based */
	cpu++;

	entry ("number", "%u of %lu", cpu, max);
	return;

unknown_sched_cpu:
#endif
	entry ("number", "%s of %lu", UNKNOWN_STR, max);
}

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
void
get_canonical (const char *path, char *canonical, size_t len)
{
	ssize_t  bytes;

	assert (path);
	assert (canonical);
	assert (len);

	bytes = readlink (path, canonical, len);
	if (bytes < 0)
		sprintf (canonical, UNKNOWN_STR);
	else
		canonical[bytes] = '\0';
}

void
get_tty_locked_status (struct termios *lock_status)
{
	assert (lock_status);
	assert (user.tty_fd != -1);

	if (ioctl (user.tty_fd, TIOCGLCKTRMIOS, lock_status) < 0)
		die ("failed to query terminal lock status");
}

#endif /* PROCENV_LINUX */

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

	entry ("number", "%u of %lu", cpu, max);
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

	/* compiler appeasement */
	return -1;
}

int
get_output_format (const char *name)
{
	struct procenv_map *p;

	assert (name);

	for (p = output_format_map; p && p->name; p++) {
		if (! strcmp (name, p->name)) {
			return p->num;
		}
	}
	die ("invalid output format value: '%s'", name);

	/* compiler appeasement */
	return -1;
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

int
get_major_minor (const char *path, unsigned int *_major, unsigned int *_minor)
{
	struct stat  st;

	assert (path);
	assert (_major);
	assert (_minor);

	if (stat (path, &st) < 0) {
		/* Don't fail as this query may be for a mount which the
		 * user does not have permission to check.
		 */
		*_major = *_minor = 0;
		return FALSE;
	}

	*_major = major (st.st_dev);
	*_minor = minor (st.st_dev);

	return TRUE;
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
		char    cwd[PATH_MAX];
		size_t  bytes;
		size_t  len;

		memset (cwd, '\0', sizeof (cwd));

		bytes = sizeof (cwd);

		/* relative path */
		assert (getcwd (cwd, bytes));
		len = strlen (cwd);

		bytes -= len;

		strncat (cwd, "/", bytes);

		bytes -= strlen ("/");

		strncat (cwd, argv0, bytes);
		cwd[sizeof (cwd) - 1] = '\0';

		if (! stat (cwd, &statbuf))
			return strdup (cwd);
		return NULL;
	}

	/* path search required */
	tmp = getenv ("PATH");
	path = strdup (tmp ? tmp : _PATH_STDPATH);
	assert (path);

	tmp = path;
	for (element = strsep (&tmp, ":");
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

void
show_threads (void)
{
	size_t              stack_size = 0;
	size_t              guard_size = 0;
	pthread_attr_t      attr;
	int                 scope;
	int                 sched;
	int                 inherit_sched;
	int                 ret;
	struct sched_param  param;

	header ("threads");

	/* cannot fail */
	(void) pthread_attr_init (&attr);
	(void) pthread_attr_getstacksize (&attr, &stack_size);

	entry ("stack size", "%lu bytes",
			(unsigned long int)stack_size);

#if defined (PROCENV_ANDROID)
	ret = 0;
	scope = pthread_attr_getscope (&attr);
#else
	ret = pthread_attr_getscope (&attr, &scope);
#endif
	entry ("scope", "%s",
			ret != 0 ? UNKNOWN_STR :
			scope == PTHREAD_SCOPE_SYSTEM ? "PTHREAD_SCOPE_SYSTEM"
			: "PTHREAD_SCOPE_PROCESS");

	ret = pthread_attr_getguardsize (&attr, &guard_size);
	if (ret == 0) {
		entry ("guard size", "%lu bytes",
				(unsigned long int)guard_size);
	} else {
		entry ("guard size", "%s", UNKNOWN_STR);
	}

	ret = pthread_getschedparam (pthread_self (), &sched, &param);

	section_open ("scheduler");

	entry ("type", "%s",
			ret != 0
			? UNKNOWN_STR
			: get_thread_scheduler_name (sched));

	if (ret != 0)
		entry ("priority", "%s", UNKNOWN_STR);
	else
		entry ("priority", "%d", param.sched_priority);

#ifdef PROCENV_ANDROID
	section_close ();
#else
	ret = pthread_attr_getinheritsched (&attr, &inherit_sched);
	entry ("inherit-scheduler attribute", "%s",
			ret != 0 ? UNKNOWN_STR :
			inherit_sched == PTHREAD_INHERIT_SCHED
			?  "PTHREAD_INHERIT_SCHED"
			: "PTHREAD_EXPLICIT_SCHED");

	section_close ();

	entry ("concurrency", "%d", pthread_getconcurrency ());
#endif

	footer ();
}

char *
get_thread_scheduler_name (int sched)
{
	struct procenv_map *p;

	for (p = thread_sched_policy_map; p && p->name; p++) {
		if (p->num == sched)
			return p->name;
	}

	return NULL;
}

#define DATA_MODEL(array, i, l, p) \
	(array[0] == i && array[1] == l && array[2] == p)

void
show_data_model (void)
{
	int	 ilp[3];
	char     data_model[8];
	size_t   pointer_size;

	ilp[0] = sizeof (int);
	ilp[1] = sizeof (long);
	ilp[2] = sizeof (void *);

	pointer_size = ilp[2];

	if (DATA_MODEL (ilp, 2, 4, 4))
		sprintf (data_model, "LP32");
	else if (DATA_MODEL (ilp, 4, 4, 4))
		sprintf (data_model, "ILP32");
	else if (DATA_MODEL (ilp, 4, 4, 8) && sizeof (long long) == pointer_size)
		sprintf (data_model, "LLP64");
	else if (DATA_MODEL (ilp, 4, 8, 8))
		sprintf (data_model, "LP64");
	else if (DATA_MODEL (ilp, 8, 8, 8))
		sprintf (data_model, "ILP64");
	else
		sprintf (data_model, UNKNOWN_STR);

	if (pointer_size > 8)
		die ("%d-byte pointers not supported", (int)pointer_size);

	entry ("data model", "%s (%d/%d/%d)",
			data_model,
			ilp[0], ilp[1], ilp[2]);
}
#undef DATA_MODEL

int
main (int    argc,
      char  *argv[])
{
	int    option;
	int    long_index;
	int    done = FALSE;

	struct option long_options[] = {
		{"meta"            , no_argument, NULL, 'a'},
		{"arguments"       , no_argument, NULL, 'A'},
		{"libs"            , no_argument, NULL, 'b'},
		{"cgroups"         , no_argument, NULL, 'c'},
		{"cpu"             , no_argument, NULL, 'C'},
		{"compiler"        , no_argument, NULL, 'd'},
		{"environment"     , no_argument, NULL, 'e'},
		{"semaphores"      , no_argument, NULL, 'E'},
		{"fds"             , no_argument, NULL, 'f'},
		{"sizeof"          , no_argument, NULL, 'g'},
		{"help"            , no_argument, NULL, 'h'},
		{"misc"            , no_argument, NULL, 'i'},
		{"uname"           , no_argument, NULL, 'j'},
		{"clocks"          , no_argument, NULL, 'k'},
		{"limits"          , no_argument, NULL, 'l'},
		{"locale"          , no_argument, NULL, 'L'},
		{"mounts"          , no_argument, NULL, 'm'},
		{"message-queues"  , no_argument, NULL, 'M'},
		{"confstr"         , no_argument, NULL, 'n'},
		{"network"         , no_argument, NULL, 'N'},
		{"oom"             , no_argument, NULL, 'o'},
		{"process"         , no_argument, NULL, 'p'},
		{"platform"        , no_argument, NULL, 'P'},
		{"time"            , no_argument, NULL, 'q'},
		{"ranges"          , no_argument, NULL, 'r'},
		{"signals"         , no_argument, NULL, 's'},
		{"shared-memory"   , no_argument, NULL, 'S'},
		{"tty"             , no_argument, NULL, 't'},
		{"threads"         , no_argument, NULL, 'T'},
		{"stat"            , no_argument, NULL, 'u'},
		{"rusage"          , no_argument, NULL, 'U'},
		{"version"         , no_argument, NULL, 'v'},
		{"capabilities"    , no_argument, NULL, 'w'},
		{"pathconf"        , no_argument, NULL, 'x'},
		{"sysconf"         , no_argument, NULL, 'y'},
		{"timezone"        , no_argument, NULL, 'z'},
		{"exec"            , no_argument      , NULL, 0},
		{"file"            , required_argument, NULL, 0},
		{"format"          , required_argument, NULL, 0},
		{"indent"          , required_argument, NULL, 0},
		{"indent-char"     , required_argument, NULL, 0},
		{"output"          , required_argument, NULL, 0},
		{"separator"       , required_argument, NULL, 0},

		/* terminator */
		{NULL              , no_argument      , NULL, 0}
	};

	doc = strdup ("");

	program_name = argv[0];
	argvp = argv;
	argvc = argc;

	/* Check before command-line options, since the latter
	 * must take priority.
	 */
	check_envvars ();

	init ();

	while (TRUE) {
		option = getopt_long (argc, argv,
				"aAbcCdeEfghijklLmMnNopPqrsStTuUvwxyz",
				long_options, &long_index);
		if (option == -1)
			break;

		/* If the user has specified a display option, only
		 * display that particular group (but crucially don't
		 * count non-display options).
		 */
		if (option) {
			done = TRUE;
			master_header (&doc);
		}

		selected_option = option;

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
			} else if (! strcmp ("format", long_options[long_index].name)) {
				output_format = get_output_format (optarg);
			} else if (! strcmp ("indent", long_options[long_index].name)) {
				indent_amount = atoi(optarg);
				if (indent_amount <= 0)
					die ("cannot specify indent <= 0");
			} else if (! strcmp ("indent-char", long_options[long_index].name)) {
				/* Special character handling */
				if (! strcmp (optarg, "\\t"))
					indent_char = '\t';
				else
					indent_char = *optarg;
				if (! indent_char)
					die ("cannot use nul indent character");
			} else if (! strcmp ("separator", long_options[long_index].name)) {
				text_separator = optarg;
			}

			/* reset */
			selected_option = 0;
			indent = 0;

			break;

		case 'a':
			show_meta ();
			break;

		case 'A':
			show_arguments ();
			break;

		case 'b':
#ifndef PROCENV_ANDROID
			show_libs ();
#endif
			break;

		case 'c':
#if defined (PROCENV_LINUX)
			show_linux_cgroups ();
#endif
			break;

		case 'C':
			show_cpu ();
			break;

		case 'd':
			show_compiler ();
			break;

		case 'e':
			show_env ();
			break;

		case 'E':
			show_semaphores ();
			break;

		case 'f':
			show_fds ();
			break;

		case 'g':
			show_sizeof ();
			break;

		case 'h':
			usage ();
			break;

		case 'i':
			get_uname ();
			get_user_info ();
			get_misc ();
			show_misc ();
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

		case 'L':
			show_locale ();
			break;

		case 'm':
			show_mounts (SHOW_MOUNTS);
			break;

		case 'M':
			show_msg_queues ();
			break;

		case 'n':
#ifndef PROCENV_ANDROID
			show_confstrs ();
#endif
			break;

		case 'N':
			show_network ();
			break;

		case 'o':
#if defined (PROCENV_LINUX)
			show_oom ();
#endif
			break;

		case 'p':
			get_misc ();
			show_proc ();
			break;

		case 'P':
			show_platform ();
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

		case 'S':
			show_shared_mem ();
			break;

		case 't':
			show_tty_attrs ();
			break;

		case 'T':
			show_threads ();
			break;

		case 'u':
			show_stat ();
			break;

		case 'U':
			show_rusage ();
			break;

		case 'v':
			show_version ();
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
			show_sysconf ();
			break;

		case 'z':
			show_timezone ();
			break;

		case '?':
			die ("invalid option '%c' specified", option);
			break;
		}
	}

	if (done) {
		common_assert ();

		master_footer (&doc);

		chomp (doc);
		compress (&doc);

		_show_output (doc);

		goto finish;
	}

	if (output == OUTPUT_SYSLOG)
		openlog (PACKAGE_NAME, LOG_CONS | LOG_PID, LOG_USER);

	if (reexec && ! exec_args && optind >= argc)
		die ("must specify atleast one argument with '--exec'");

	/* Prepare for re-exec */
	if (reexec) {
		if (! exec_args) {
			argv += optind;
			exec_args = argv;
		}
	}

	dump ();

	chomp (doc);
	compress (&doc);

	_show_output (doc);

finish:
	cleanup ();

	/* Perform re-exec */
	if (reexec) {
		execvp (exec_args[0], exec_args);
		die ("failed to re-exec %s", exec_args[0]);
	}

	exit (EXIT_SUCCESS);

}

char *
get_user_name (uid_t uid)
{
	struct passwd *p;
       
	p = getpwuid (uid);

	return p ? p->pw_name : NULL;
}

char *
get_group_name (gid_t gid)
{
	struct group *g;
	       
	g = getgrgid (gid);

	return g ? g->gr_name : NULL;
}

/**
 * encode_string:
 *
 * Returns: 0 on success, -1 on failure.
 *
 * Note: It is the callers responsibility to free @str iff this function
 * is successful. any previous value of @str will be freed by
 * encode_string().
 **/
int
encode_string (char **str)
{
	int    ret = 0;
	char   *new;

	assert (str);
	assert (*str);

	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		/* Nothing to do */
		ret = 0;
		break;

	case OUTPUT_FORMAT_JSON: /* FALL THROUGH */
	case OUTPUT_FORMAT_XML:
		new = translate (*str);
		if (new) {
			free (*str);
			*str = new;
		} else {
			ret = -1;
		}
		break;

	default:
		assert_not_reached ();
		break;
	}

	return ret;
}

char *
translate (const char *str)
{
	char               *result = NULL;
	const char         *start;
	const char         *p;
	TranslateTable     *table;
	size_t              i;
	size_t              len;
	char                from;

	assert (str);
	assert (output_format != OUTPUT_FORMAT_TEXT);

	len = 1 + strlen (str);
	start = str;

	/* Find the correct translation table for the chosen output format */
	for (i = 0; i < sizeof (translate_table) / sizeof (translate_table[0]); i++) {
		table = &translate_table[i];
		if (table && table->output_format == output_format)
			break;
	}

	if (! table)
		return NULL;

	/* First, calculate the amount of space needed for the expanded
	 * buffer.
	 */
	while (start && *start) {
		for (i = 0; i < TRANSLATE_MAP_ENTRIES; i++) {
			from = table->map[i].from;
			if (*start == from) {
				len += strlen (table->map[i].to);
			}
		}
		start++;
	}

	result = calloc (len, 1);
	if (! result)
		return NULL;

	/* Now, iterate the string again, performing the actual
	 * replacements.
	 */
	p = start = str;

	while (p && *p) {
		for (i = 0; i < TRANSLATE_MAP_ENTRIES; i++) {
			from = table->map[i].from;
			if (*p == from) {
				size_t   amount;
				char    *to;

				to = table->map[i].to;

				amount = (p - start);

				/* Copy from start to match */
				strncat (result, start, amount);

				/* Nudge along the string, jumping over
				 * matching character.
				 */
				start += (amount+1);

				/* Copy replacement text */
				strncat (result, to, strlen (to));

				break;
			}
		}
		p++;
	}

	/* Copy remaining non-matching chars */
	strncat (result, start, (p - start));

	return result;
}

/**
 * change_element:
 *
 * Handle changing to a new element type. Depending on the output
 * format, this may require separators and newlines to be emitted to
 * produce well-formatted output.
 **/
void
change_element (ElementType new)
{
	common_assert ();

	last_element = current_element;

	current_element = new;

	if (current_element == ELEMENT_TYPE_CONTAINER_OPEN)
		in_container = TRUE;
	if (current_element == ELEMENT_TYPE_CONTAINER_CLOSE)
		in_container = FALSE;

	format_element ();
}

void
format_element (void)
{
	switch (output_format) {

	case OUTPUT_FORMAT_TEXT:
		format_text_element ();
		break;

	case OUTPUT_FORMAT_JSON:
		format_json_element ();
		break;

	case OUTPUT_FORMAT_XML:
		format_xml_element ();
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
format_text_element (void)
{
	common_assert ();
	switch (last_element) {

	case ELEMENT_TYPE_ENTRY:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN:
				append (&doc, "\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN:
				append (&doc, "\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				add_indent (&doc);
				break;

				/* Format, but don't adjust indent */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				append (&doc, "\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN:
			case ELEMENT_TYPE_OBJECT_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_CONTAINER_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_OPEN:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_NONE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
format_json_element (void)
{
	common_assert ();

	switch (last_element) {

	case ELEMENT_TYPE_ENTRY:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, ",\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE:
			case ELEMENT_TYPE_OBJECT_OPEN:
				/* NOP */
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				assert_not_reached ();
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, ",\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_OPEN:
				/* NOP */
				break;

			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN:
				append (&doc, "\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_CLOSE:
			case ELEMENT_TYPE_SECTION_CLOSE:
				assert_not_reached ();
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, ",\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN:
				append (&doc, "\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_CONTAINER_CLOSE:
			case ELEMENT_TYPE_SECTION_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_OPEN:
			case ELEMENT_TYPE_SECTION_OPEN:
				append (&doc, ",\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_NONE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	default:
		assert_not_reached ();
		break;
	}
}

void
format_xml_element (void)
{
	common_assert ();

	switch (last_element) {

	case ELEMENT_TYPE_ENTRY:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE:
				append (&doc, "\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				assert_not_reached ();
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_SECTION_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_SECTION_CLOSE:
				assert_not_reached ();
				break;

			case ELEMENT_TYPE_CONTAINER_CLOSE:
				append (&doc, "\n");
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_OPEN:
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_CONTAINER_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				append (&doc, "\n");
				dec_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_OPEN:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN:
				append (&doc, "\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_CONTAINER_OPEN:
				append (&doc, "\n");
				inc_indent ();
				add_indent (&doc);
				break;

			case ELEMENT_TYPE_OBJECT_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_OBJECT_CLOSE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE:
				/* NOP */
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	case ELEMENT_TYPE_NONE:
		{
			switch (current_element) {
			case ELEMENT_TYPE_ENTRY: /* FALL */
			case ELEMENT_TYPE_CONTAINER_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_OPEN: /* FALL */
			case ELEMENT_TYPE_SECTION_CLOSE: /* FALL */
			case ELEMENT_TYPE_CONTAINER_CLOSE: /* FALL */
			case ELEMENT_TYPE_OBJECT_OPEN: /* FALL */
			case ELEMENT_TYPE_OBJECT_CLOSE:
				add_indent (&doc);
				break;

			default:
				assert_not_reached ();
				break;
			}
		}
		break;

	default:
		assert_not_reached ();
		break;
	}
}

/**
 * compress:
 *
 * Remove lines composed entirely of whitespace from @str.
 *
 * This is required specifically for '--output=text' which in some
 * scenarios generates lines comprising pure whitespace. This is
 * unecessary and results from the fact that when an
 * ELEMENT_TYPE_OBJECT_* is encountered, formatting is applied for the
 * previously seen element, but sometimes such "objects" should be
 * invisible.
 **/
void
compress (char **str)
{
#define NUMBER_MATCHES 1
	char         *new = NULL;
	regex_t       regex;
	regmatch_t    matches[NUMBER_MATCHES];
	regmatch_t   *match;
	char         *start;
	char         *pattern = NULL;
	int           ret;

	assert (str);
	assert (*str);

	start = *str;

	/* Match lines composed entirely of indent_char chars, and
	 * entirely blank lines.
	 */
	appendf (&pattern, "^([%c][%c]*$|^$)", indent_char, indent_char);

	if (regcomp (&regex, pattern, REG_EXTENDED|REG_NEWLINE))
		goto error;

	while (start && *start) {

		ret = regexec (&regex, start, NUMBER_MATCHES, matches, 0);
		if (ret == REG_NOMATCH)
			break;

		match = &matches[0];
		if (match->rm_so == -1)
			break;

		/* Newlines annoyingly need to be special-cased
		 * (due to the "^$" pattern) to avoid
		 * looping when the input contains say "\n\n".
		 */
		if (! match->rm_so && ! match->rm_eo && start[match->rm_so] == '\n') {
			start++;
			continue;
		}

		/* Copy text before the match to the output buffer */
		appendn (&new, start, match->rm_so);

		/* Jump over the matching chars */
		start += match->rm_eo;
	}

	regfree (&regex);

	if (new) {
		free (*str);
		*str = new;
	}

	free (pattern);
	return;

error:
	free (pattern);
	die ("failed to compile regex");
}

/**
 * chomp:
 *
 * Remove trailing extraneous newlines and indent_chars from @str.
 **/
void
chomp (char *str)
{
	size_t  len;
	int     removable = 0;
	char   *p;

	assert (str);

	len = strlen (str);

	/* Unable to add '\n' in this scenario */
	if (len < 2)
		return;

	for (p = str+len-1; *p == '\n' || *p == (char)indent_char;
			p--, removable++)
		;

	/* Chop string at the appropriate place after first adding a new
	 * newline.
	 */
	if (removable > 1) {
		len -= (removable-1);
		str[len-1] = '\n';
		str[len] = '\0';
	}
}

void
show_version (void)
{
	common_assert ();

	header ("version");

	entry (_("name"), "%s", PACKAGE_NAME);
	entry (_("version"), "%s", PACKAGE_VERSION);
	entry (_("author"), "%s", PROGRAM_AUTHORS);
	
	footer ();
}

void
show_shared_mem (void)
{
#ifdef PROCENV_LINUX
	show_shared_mem_linux ();
#else
	show_shared_mem_bsd ();
#endif
}

void
show_semaphores (void)
{
#ifdef PROCENV_LINUX
	show_semaphores_linux ();
#else
	show_semaphores_bsd ();
#endif
}

void
show_msg_queues (void)
{
#ifdef PROCENV_LINUX
	show_msg_queues_linux ();
#else
	show_msg_queues_bsd ();
#endif
}


#if defined (PROCENV_LINUX)
void
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

	header ("shared memory");

	max = shmctl (0, SHM_INFO, (struct shmid_ds *)&info);
	if (max < 0)
		goto out;

	/* Display summary details */

	section_open ("info");

	entry ("segments", "%u", info.used_ids);
	entry ("pages", "%lu", info.shm_tot);
	entry ("shm_rss", "%lu", info.shm_rss);
	entry ("shm_swp", "%lu", info.shm_swp);

	section_close ();

	container_open ("segments");

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

#ifdef PROCENV_LINUX
		locked = (perm->mode & SHM_LOCKED);
		destroy = (perm->mode & SHM_DEST);
#endif

		format_time (&shmid_ds.shm_atime, formatted_atime, sizeof (formatted_atime));
		format_time (&shmid_ds.shm_ctime, formatted_ctime, sizeof (formatted_ctime));
		format_time (&shmid_ds.shm_dtime, formatted_dtime, sizeof (formatted_dtime));

		cpid = pid_to_name (shmid_ds.shm_cpid);
		lpid = pid_to_name (shmid_ds.shm_lpid);

		appendf (&id_str, "%d", id);

		container_open (id_str);
		free (id_str);

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
		entry ("last", "%d (%s)", shmid_ds.shm_cpid, lpid ? lpid : UNKNOWN_STR);
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

		container_close ();

		free (modestr);
		if (cpid)
			free (cpid);
		if (lpid)
			free (lpid);
	}

	container_close ();

out:
    footer ();
}

void
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

	header ("semaphores");

	max = semctl (0, 0, SEM_INFO, &info);
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

		/* pad out to max pointer size represented in hex.
		 */
		entry ("key", "0x%.*x", POINTER_SIZE * 2, perm->__key);
		entry ("sequence", "%u", perm->__seq);

		entry ("number in set", "%lu", semid_ds.sem_nsems);

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
		entry ("last semop (otime)", "%lu (%s)", semid_ds.sem_otime, formatted_otime);
		entry ("last change (ctime)", "%lu (%s)", semid_ds.sem_ctime, formatted_ctime);
		section_close ();

		container_close ();
	}

	container_close ();

out:
	footer ();
}

void
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

	header ("message queues");

	max = msgctl (0, MSG_INFO, (struct msqid_ds  *)&info);
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

		container_close ();

		free (modestr);
		if (lspid)
			free (lspid);
		if (lrpid)
			free (lrpid);
	}

	container_close ();

out:
	footer ();
}
#endif /* PROCENV_LINUX */

void
format_time (const time_t *t, char *buffer, size_t len)
{
	char   *str = NULL;
	size_t  l;

	assert (t);
	assert (buffer);

	str = ctime (t);
	if (! str)
		die ("failed to format time");

	l = strlen (str);

	if (len < l)
		bug ("buffer too small");

	/* Ensure nul byte copied */
	strncpy (buffer, str, l+1);

	/* Overwrite NL */
	buffer[strlen (buffer)-1] = '\0';
}

char *
format_perms (mode_t mode)
{
	char    *modestr = NULL;
	mode_t   perms;
	int      i = 0;

	/*
	 * "-rwxrwxrwx" = 10+1 bytes.
	 */
	modestr = calloc ((1+3+3+3)+1, 1);

	if (! modestr)
		return NULL;

	modestr[i++] = (S_ISLNK (mode & S_IFMT)) ? 'l' : '-';

	perms = (mode & S_IRWXU);
	modestr[i++] = (perms & S_IRUSR) ? 'r' : '-';
	modestr[i++] = (perms & S_IWUSR) ? 'w' : '-';
	modestr[i++] = (perms & S_IXUSR) ? 'x' : '-';

	perms = (mode & S_IRWXG);
	modestr[i++] = (perms & S_IRGRP) ? 'r' : '-';
	modestr[i++] = (perms & S_IWGRP) ? 'w' : '-';
	modestr[i++] = (perms & S_IXGRP) ? 'x' : '-';

	perms = (mode & S_IRWXO);
	modestr[i++] = (perms & S_IROTH) ? 'r' : '-';
	modestr[i++] = (perms & S_IWOTH) ? 'w' : '-';
	modestr[i++] = (perms & S_IXOTH) ? 'x' : '-';

	perms = (mode &= ~S_IFMT);
	if (perms & S_ISUID)
		modestr[3] = 's';
	if (perms & S_ISGID)
		modestr[6] = 's';
	if (perms & S_ISVTX)
		modestr[9] = 't';

	return modestr;
}

char *
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
