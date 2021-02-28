/*--------------------------------------------------------------------
 * Description: Simple program to display information on process
 *              environment.
 *
 * Date: 24 October 2012.
 *
 * Author: James Hunt <jamesodhunt@gmail.com>.
 *
 * Licence: GPLv3. See below...
 *--------------------------------------------------------------------
 *
 * Copyright © 2012-2016 James Hunt <jamesodhunt@gmail.com>.
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

/**
 * doc:
 *
 * The output document in wide-character format.
 **/
pstring *doc = NULL;

/**
 * reexec:
 *
 * true if we should re-exec at the end.
 **/
int reexec = false;

/**
 * selected_option:
 *
 * A non-zero value denotes user has requested a subset of the available
 * information with the value representing the short command-line
 * option.
 **/
int selected_option = 0;


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
 * Locale in effect at program startup.
 **/
char *saved_locale = NULL;

/* Get a handle to the platform-specific routines */
extern struct procenv_ops platform_ops;
struct procenv_ops *ops = &platform_ops;

extern wchar_t wide_indent_char;
extern int indent;

struct procenv_user     user;
struct procenv_misc     misc;
struct procenv_priority priority;
struct procenv_priority priority_io;

struct utsname uts;

/* Really, every single sysconf variable should be ifdef'ed since it
 * may not exist on a particular system, but that makes the code look
 * untidy.
 *
 * The ifdefs we have below seem sufficient for modern systems
 * (Ubuntu Lucid and newer). If only we could use ifdef in macros.
 * sigh...
 */
struct procenv_map sysconf_map[] = {

#if defined (_SC_2_C_BIND)
	mk_sysconf_map_entry (_SC_2_C_BIND),
#endif

#if defined (_SC_2_C_DEV)
	mk_sysconf_map_entry (_SC_2_C_DEV),
#endif

#if defined (_SC_2_CHAR_TERM)
	mk_sysconf_map_entry (_SC_2_CHAR_TERM),
#endif

#if defined (_SC_2_C_VERSION)
	mk_sysconf_map_entry (_SC_2_C_VERSION),
#endif

#if defined (_SC_2_FORT_DEV)
	mk_sysconf_map_entry (_SC_2_FORT_DEV),
#endif

#if defined (_SC_2_FORT_RUN)
	mk_sysconf_map_entry (_SC_2_FORT_RUN),
#endif

#if defined (_SC_2_LOCALEDEF)
	mk_sysconf_map_entry (_SC_2_LOCALEDEF),
#endif

#if defined (_SC_2_PBS)
	mk_sysconf_map_entry (_SC_2_PBS),
#endif

#if defined (_SC_2_PBS_ACCOUNTING)
	mk_sysconf_map_entry (_SC_2_PBS_ACCOUNTING),
#endif

#if defined (_SC_2_PBS_LOCATE)
	mk_sysconf_map_entry (_SC_2_PBS_LOCATE),
#endif

#if defined (_SC_2_PBS_MESSAGE)
	mk_sysconf_map_entry (_SC_2_PBS_MESSAGE),
#endif

#if defined (_SC_2_PBS_TRACK)
	mk_sysconf_map_entry (_SC_2_PBS_TRACK),
#endif

#if defined (_SC_2_SW_DEV)
	mk_sysconf_map_entry (_SC_2_SW_DEV),
#endif

#if defined (_SC_2_UPE)
	mk_sysconf_map_entry (_SC_2_UPE),
#endif

#if defined (_SC_2_VERSION)
	mk_sysconf_map_entry (_SC_2_VERSION),
#endif

#if defined (_SC_ADVISORY_INFO)
	mk_sysconf_map_entry (_SC_ADVISORY_INFO),
#endif

#if defined (_SC_AIO_LISTIO_MAX)
	mk_sysconf_map_entry (_SC_AIO_LISTIO_MAX),
#endif

#if defined (_SC_AIO_MAX)
	mk_sysconf_map_entry (_SC_AIO_MAX),
#endif

#if defined (_SC_AIO_PRIO_DELTA_MAX)
	mk_sysconf_map_entry (_SC_AIO_PRIO_DELTA_MAX),
#endif

#if defined (_SC_ARG_MAX)
	mk_sysconf_map_entry (_SC_ARG_MAX),
#endif

#if defined (_SC_ASYNCHRONOUS_IO)
	mk_sysconf_map_entry (_SC_ASYNCHRONOUS_IO),
#endif

#if defined (_SC_ATEXIT_MAX)
	mk_sysconf_map_entry (_SC_ATEXIT_MAX),
#endif

#if defined (_SC_AVPHYS_PAGES)
	mk_sysconf_map_entry (_SC_AVPHYS_PAGES),
#endif

#if defined (_SC_BARRIERS)
	mk_sysconf_map_entry (_SC_BARRIERS),
#endif

#if defined (_SC_BASE)
	mk_sysconf_map_entry (_SC_BASE),
#endif

#if defined (_SC_BC_BASE_MAX)
	mk_sysconf_map_entry (_SC_BC_BASE_MAX),
#endif

#if defined (_SC_BC_DIM_MAX)
	mk_sysconf_map_entry (_SC_BC_DIM_MAX),
#endif

#if defined (_SC_BC_SCALE_MAX)
	mk_sysconf_map_entry (_SC_BC_SCALE_MAX),
#endif

#if defined (_SC_BC_STRING_MAX)
	mk_sysconf_map_entry (_SC_BC_STRING_MAX),
#endif

#if defined (_SC_CHAR_BIT)
	mk_sysconf_map_entry (_SC_CHAR_BIT),
#endif

#if defined (_SC_CHARCLASS_NAME_MAX)
	mk_sysconf_map_entry (_SC_CHARCLASS_NAME_MAX),
#endif

#if defined (_SC_CHAR_MAX)
	mk_sysconf_map_entry (_SC_CHAR_MAX),
#endif

#if defined (_SC_CHAR_MIN)
	mk_sysconf_map_entry (_SC_CHAR_MIN),
#endif

#if defined (_SC_CHILD_MAX)
	mk_sysconf_map_entry (_SC_CHILD_MAX),
#endif

#if defined (_SC_C_LANG_SUPPORT)
	mk_sysconf_map_entry (_SC_C_LANG_SUPPORT),
#endif

#if defined (_SC_C_LANG_SUPPORT_R)
	mk_sysconf_map_entry (_SC_C_LANG_SUPPORT_R),
#endif

#if defined (_SC_CLK_TCK)
	mk_sysconf_map_entry (_SC_CLK_TCK),
#endif

#if defined (_SC_CLOCK_SELECTION)
	mk_sysconf_map_entry (_SC_CLOCK_SELECTION),
#endif

#if defined (_SC_COLL_WEIGHTS_MAX)
	mk_sysconf_map_entry (_SC_COLL_WEIGHTS_MAX),
#endif

#if defined (_SC_CPUTIME)
	mk_sysconf_map_entry (_SC_CPUTIME),
#endif

#if defined (_SC_DELAYTIMER_MAX)
	mk_sysconf_map_entry (_SC_DELAYTIMER_MAX),
#endif

#if defined (_SC_DEVICE_IO)
	mk_sysconf_map_entry (_SC_DEVICE_IO),
#endif

#if defined (_SC_DEVICE_SPECIFIC)
	mk_sysconf_map_entry (_SC_DEVICE_SPECIFIC),
#endif

#if defined (_SC_DEVICE_SPECIFIC_R)
	mk_sysconf_map_entry (_SC_DEVICE_SPECIFIC_R),
#endif

#if defined (_SC_EQUIV_CLASS_MAX)
	mk_sysconf_map_entry (_SC_EQUIV_CLASS_MAX),
#endif

#if defined (_SC_EXPR_NEST_MAX)
	mk_sysconf_map_entry (_SC_EXPR_NEST_MAX),
#endif

#if defined (_SC_FD_MGMT)
	mk_sysconf_map_entry (_SC_FD_MGMT),
#endif

#if defined (_SC_FIFO)
	mk_sysconf_map_entry (_SC_FIFO),
#endif

#if defined (_SC_FILE_ATTRIBUTES)
	mk_sysconf_map_entry (_SC_FILE_ATTRIBUTES),
#endif

#if defined (_SC_FILE_LOCKING)
	mk_sysconf_map_entry (_SC_FILE_LOCKING),
#endif

#if defined (_SC_FILE_SYSTEM)
	mk_sysconf_map_entry (_SC_FILE_SYSTEM),
#endif

#if defined (_SC_FSYNC)
	mk_sysconf_map_entry (_SC_FSYNC),
#endif

#if defined (_SC_GETGR_R_SIZE_MAX)
	mk_sysconf_map_entry (_SC_GETGR_R_SIZE_MAX),
#endif

#if defined (_SC_GETPW_R_SIZE_MAX)
	mk_sysconf_map_entry  (_SC_GETPW_R_SIZE_MAX),
#endif

#if defined (_SC_HOST_NAME_MAX)
	mk_sysconf_map_entry (_SC_HOST_NAME_MAX),
#endif

#if defined (_SC_INT_MAX)
	mk_sysconf_map_entry (_SC_INT_MAX),
#endif

#if defined (_SC_INT_MIN)
	mk_sysconf_map_entry (_SC_INT_MIN),
#endif

#if defined (_SC_IOV_MAX)
	mk_sysconf_map_entry (_SC_IOV_MAX),
#endif

#if defined (_SC_IPV6)
	mk_sysconf_map_entry (_SC_IPV6),
#endif

#if defined (_SC_JOB_CONTROL)
	mk_sysconf_map_entry (_SC_JOB_CONTROL),
#endif

#if defined (_SC_LEVEL1_DCACHE_ASSOC)
	mk_sysconf_map_entry (_SC_LEVEL1_DCACHE_ASSOC),
#endif

#if defined (_SC_LEVEL1_DCACHE_LINESIZE)
	mk_sysconf_map_entry (_SC_LEVEL1_DCACHE_LINESIZE),
#endif

#if defined (_SC_LEVEL1_DCACHE_SIZE)
	mk_sysconf_map_entry (_SC_LEVEL1_DCACHE_SIZE),
#endif

#if defined (_SC_LEVEL1_ICACHE_ASSOC)
	mk_sysconf_map_entry (_SC_LEVEL1_ICACHE_ASSOC),
#endif

#if defined (_SC_LEVEL1_ICACHE_LINESIZE)
	mk_sysconf_map_entry (_SC_LEVEL1_ICACHE_LINESIZE),
#endif

#if defined (_SC_LEVEL1_ICACHE_SIZE)
	mk_sysconf_map_entry (_SC_LEVEL1_ICACHE_SIZE),
#endif

#if defined (_SC_LEVEL2_CACHE_ASSOC)
	mk_sysconf_map_entry (_SC_LEVEL2_CACHE_ASSOC),
#endif

#if defined (_SC_LEVEL2_CACHE_LINESIZE)
	mk_sysconf_map_entry (_SC_LEVEL2_CACHE_LINESIZE),
#endif

#if defined (_SC_LEVEL2_CACHE_SIZE)
	mk_sysconf_map_entry (_SC_LEVEL2_CACHE_SIZE),
#endif

#if defined (_SC_LEVEL3_CACHE_ASSOC)
	mk_sysconf_map_entry (_SC_LEVEL3_CACHE_ASSOC),
#endif

#if defined (_SC_LEVEL3_CACHE_LINESIZE)
	mk_sysconf_map_entry (_SC_LEVEL3_CACHE_LINESIZE),
#endif

#if defined (_SC_LEVEL3_CACHE_SIZE)
	mk_sysconf_map_entry (_SC_LEVEL3_CACHE_SIZE),
#endif

#if defined (_SC_LEVEL4_CACHE_ASSOC)
	mk_sysconf_map_entry (_SC_LEVEL4_CACHE_ASSOC),
#endif

#if defined (_SC_LEVEL4_CACHE_LINESIZE)
	mk_sysconf_map_entry (_SC_LEVEL4_CACHE_LINESIZE),
#endif

#if defined (_SC_LEVEL4_CACHE_SIZE)
	mk_sysconf_map_entry (_SC_LEVEL4_CACHE_SIZE),
#endif

#if defined (_SC_LINE_MAX)
	mk_sysconf_map_entry (_SC_LINE_MAX),
#endif

#if defined (_SC_LOGIN_NAME_MAX)
	mk_sysconf_map_entry (_SC_LOGIN_NAME_MAX),
#endif

#if defined (_SC_LONG_BIT)
	mk_sysconf_map_entry (_SC_LONG_BIT),
#endif

#if defined (_SC_MAPPED_FILES)
	mk_sysconf_map_entry (_SC_MAPPED_FILES),
#endif

#if defined (_SC_MB_LEN_MAX)
	mk_sysconf_map_entry (_SC_MB_LEN_MAX),
#endif

#if defined (_SC_MEMLOCK)
	mk_sysconf_map_entry (_SC_MEMLOCK),
#endif

#if defined (_SC_MEMLOCK_RANGE)
	mk_sysconf_map_entry (_SC_MEMLOCK_RANGE),
#endif

#if defined (_SC_MEMORY_PROTECTION)
	mk_sysconf_map_entry (_SC_MEMORY_PROTECTION),
#endif

#if defined (_SC_MESSAGE_PASSING)
	mk_sysconf_map_entry (_SC_MESSAGE_PASSING),
#endif

#if defined (_SC_MONOTONIC_CLOCK)
	mk_sysconf_map_entry (_SC_MONOTONIC_CLOCK),
#endif

#if defined (_SC_MQ_OPEN_MAX)
	mk_sysconf_map_entry (_SC_MQ_OPEN_MAX),
#endif

#if defined (_SC_MQ_PRIO_MAX)
	mk_sysconf_map_entry (_SC_MQ_PRIO_MAX),
#endif

#if defined (_SC_MULTI_PROCESS)
	mk_sysconf_map_entry (_SC_MULTI_PROCESS),
#endif

#if defined (_SC_NETWORKING)
	mk_sysconf_map_entry (_SC_NETWORKING),
#endif

#if defined (_SC_NGROUPS_MAX)
	mk_sysconf_map_entry (_SC_NGROUPS_MAX),
#endif

#if defined (_SC_NL_ARGMAX)
	mk_sysconf_map_entry (_SC_NL_ARGMAX),
#endif

#if defined (_SC_NL_LANGMAX)
	mk_sysconf_map_entry (_SC_NL_LANGMAX),
#endif

#if defined (_SC_NL_MSGMAX)
	mk_sysconf_map_entry (_SC_NL_MSGMAX),
#endif

#if defined (_SC_NL_NMAX)
	mk_sysconf_map_entry (_SC_NL_NMAX),
#endif

#if defined (_SC_NL_SETMAX)
	mk_sysconf_map_entry (_SC_NL_SETMAX),
#endif

#if defined (_SC_NL_TEXTMAX)
	mk_sysconf_map_entry (_SC_NL_TEXTMAX),
#endif

#if defined (_SC_NPROCESSORS_CONF)
	mk_sysconf_map_entry (_SC_NPROCESSORS_CONF),
#endif

#if defined (_SC_NPROCESSORS_ONLN)
	mk_sysconf_map_entry (_SC_NPROCESSORS_ONLN),
#endif

#if defined (_SC_NZERO)
	mk_sysconf_map_entry (_SC_NZERO),
#endif

#if defined (_SC_OPEN_MAX)
	mk_sysconf_map_entry (_SC_OPEN_MAX),
#endif

#if defined (_SC_PAGESIZE)
	mk_sysconf_map_entry (_SC_PAGESIZE),
#endif

#if defined (_SC_PAGE_SIZE)
	mk_sysconf_map_entry (_SC_PAGE_SIZE),
#endif

#if defined (_SC_PASS_MAX)
	mk_sysconf_map_entry (_SC_PASS_MAX),
#endif

#if defined (_SC_PHYS_PAGES)
	mk_sysconf_map_entry (_SC_PHYS_PAGES),
#endif

#if defined (_SC_PII)
	mk_sysconf_map_entry (_SC_PII),
#endif

#if defined (_SC_PII_INTERNET)
	mk_sysconf_map_entry (_SC_PII_INTERNET),
#endif

#if defined (_SC_PII_INTERNET_DGRAM)
	mk_sysconf_map_entry (_SC_PII_INTERNET_DGRAM),
#endif

#if defined (_SC_PII_INTERNET_STREAM)
	mk_sysconf_map_entry (_SC_PII_INTERNET_STREAM),
#endif

#if defined (_SC_PII_OSI)
	mk_sysconf_map_entry (_SC_PII_OSI),
#endif

#if defined (_SC_PII_OSI_CLTS)
	mk_sysconf_map_entry (_SC_PII_OSI_CLTS),
#endif

#if defined (_SC_PII_OSI_COTS)
	mk_sysconf_map_entry (_SC_PII_OSI_COTS),
#endif

#if defined (_SC_PII_OSI_M)
	mk_sysconf_map_entry (_SC_PII_OSI_M),
#endif

#if defined (_SC_PII_SOCKET)
	mk_sysconf_map_entry (_SC_PII_SOCKET),
#endif

#if defined (_SC_PII_XTI)
	mk_sysconf_map_entry (_SC_PII_XTI),
#endif

#if defined (_SC_PIPE)
	mk_sysconf_map_entry (_SC_PIPE),
#endif

#if defined (_SC_POLL)
	mk_sysconf_map_entry (_SC_POLL),
#endif

#if defined (_SC_SINGLE_PROCESS)
	mk_sysconf_map_entry (_SC_SINGLE_PROCESS),
#endif

#if defined (_SC_SYSTEM_DATABASE)
	mk_sysconf_map_entry (_SC_SYSTEM_DATABASE),
#endif

#if defined (_SC_SYSTEM_DATABASE_R)
	mk_sysconf_map_entry (_SC_SYSTEM_DATABASE_R),
#endif

#if defined (_SC_THREAD_KEYS_MAX)
	mk_sysconf_map_entry (_SC_THREAD_KEYS_MAX),
#endif

#if defined (_SC_THREAD_DESTRUCTOR_ITERATIONS)
	mk_sysconf_map_entry (_SC_THREAD_DESTRUCTOR_ITERATIONS),
#endif

#if defined (_SC_THREAD_THREADS_MAX)
	mk_sysconf_map_entry (_SC_THREAD_THREADS_MAX),
#endif

#if defined (_SC_STREAM_MAX)
	mk_sysconf_map_entry (_SC_STREAM_MAX),
#endif

#if defined (_SC_SYMLOOP_MAX)
	mk_sysconf_map_entry (_SC_SYMLOOP_MAX),
#endif

#if defined (_SC_TTY_NAME_MAX)
	mk_sysconf_map_entry (_SC_TTY_NAME_MAX),
#endif

#if defined (_SC_TZNAME_MAX)
	mk_sysconf_map_entry (_SC_TZNAME_MAX),
#endif

#if defined (_SC_TYPED_MEMORY_OBJECTS)
	mk_sysconf_map_entry (_SC_TYPED_MEMORY_OBJECTS),
#endif

#if defined (_SC_USER_GROUPS)
	mk_sysconf_map_entry (_SC_USER_GROUPS),
#endif

#if defined (_SC_USER_GROUPS_R)
	mk_sysconf_map_entry (_SC_USER_GROUPS_R),
#endif

#if defined (_SC_VERSION)
	mk_sysconf_map_entry (_SC_VERSION),
#endif

#if defined (_SC_POSIX2_C_DEV)
	mk_sysconf_map_entry (_SC_POSIX2_C_DEV),
#endif

#if defined (_SC_SIGQUEUE_MAX)
	mk_sysconf_map_entry (_SC_SIGQUEUE_MAX),
#endif

#if defined (_SC_T_IOV_MAX)
	mk_sysconf_map_entry (_SC_T_IOV_MAX),
#endif

#if defined (_SC_THREAD_STACK_MIN)
	mk_sysconf_map_entry (_SC_THREAD_STACK_MIN),
#endif

#if defined (_SC_SSIZE_MAX)
	mk_sysconf_map_entry (_SC_SSIZE_MAX),
#endif

#if defined (_SC_TIMER_MAX)
	mk_sysconf_map_entry (_SC_TIMER_MAX),
#endif

#if defined (_SC_UCHAR_MAX)
	mk_sysconf_map_entry (_SC_UCHAR_MAX),
#endif

#if defined (_SC_UINT_MAX)
	mk_sysconf_map_entry (_SC_UINT_MAX),
#endif

#if defined (_SC_UIO_MAXIOV)
	mk_sysconf_map_entry (_SC_UIO_MAXIOV),
#endif

#if defined (_SC_ULONG_MAX)
	mk_sysconf_map_entry (_SC_ULONG_MAX),
#endif

#if defined (_SC_USHRT_MAX)
	mk_sysconf_map_entry (_SC_USHRT_MAX),
#endif

#if defined (_SC_V6_LPBIG_OFFBIG)
	mk_sysconf_map_entry (_SC_V6_LPBIG_OFFBIG),
#endif

#if defined (_SC_WORD_BIT)
	mk_sysconf_map_entry (_SC_WORD_BIT),
#endif

#if defined (_SC_EXPR_NEST_MAX)
	mk_sysconf_map_entry (_SC_EXPR_NEST_MAX),
#endif

#if defined (_SC_V6_ILP32_OFF32)
	mk_sysconf_map_entry (_SC_V6_ILP32_OFF32),
#endif

#if defined (_SC_V6_ILP32_OFFBIG)
	mk_sysconf_map_entry (_SC_V6_ILP32_OFFBIG),
#endif

#if defined (_SC_V6_LP64_OFF64)
	mk_sysconf_map_entry (_SC_V6_LP64_OFF64),
#endif

#if defined (_SC_V7_ILP32_OFF32)
	mk_sysconf_map_entry (_SC_V7_ILP32_OFF32),
#endif

#if defined (_SC_V7_ILP32_OFFBIG)
	mk_sysconf_map_entry (_SC_V7_ILP32_OFFBIG),
#endif

#if defined (_SC_V7_LPBIG_OFFBIG)
	mk_sysconf_map_entry (_SC_V7_LPBIG_OFFBIG),
#endif

#if defined (_SC_V7_LP64_OFF64)
	mk_sysconf_map_entry (_SC_V7_LP64_OFF64),
#endif

#if defined (_SC_PRIORITIZED_IO)
	mk_sysconf_map_entry (_SC_PRIORITIZED_IO),
#endif

#if defined (_SC_PRIORITY_SCHEDULING)
	mk_sysconf_map_entry (_SC_PRIORITY_SCHEDULING),
#endif

#if defined (_SC_RAW_SOCKETS)
	mk_sysconf_map_entry (_SC_RAW_SOCKETS),
#endif

#if defined (_SC_READER_WRITER_LOCKS)
	mk_sysconf_map_entry (_SC_READER_WRITER_LOCKS),
#endif

#if defined (_SC_REALTIME_SIGNALS)
	mk_sysconf_map_entry (_SC_REALTIME_SIGNALS),
#endif

#if defined (_SC_RE_DUP_MAX)
	mk_sysconf_map_entry (_SC_RE_DUP_MAX),
#endif

#if defined (_SC_REGEXP)
	mk_sysconf_map_entry (_SC_REGEXP),
#endif

#if defined (_SC_REGEX_VERSION)
	mk_sysconf_map_entry (_SC_REGEX_VERSION),
#endif

#if defined (_SC_RTSIG_MAX)
	mk_sysconf_map_entry (_SC_RTSIG_MAX),
#endif

#if defined (_SC_SAVED_IDS)
	mk_sysconf_map_entry (_SC_SAVED_IDS),
#endif

#if defined (_SC_SCHAR_MAX)
	mk_sysconf_map_entry (_SC_SCHAR_MAX),
#endif

#if defined (_SC_SCHAR_MIN)
	mk_sysconf_map_entry (_SC_SCHAR_MIN),
#endif

#if defined (_SC_SELECT)
	mk_sysconf_map_entry (_SC_SELECT),
#endif

#if defined (_SC_SEMAPHORES)
	mk_sysconf_map_entry (_SC_SEMAPHORES),
#endif

#if defined (_SC_SEM_NSEMS_MAX)
	mk_sysconf_map_entry (_SC_SEM_NSEMS_MAX),
#endif

#if defined (_SC_SEM_VALUE_MAX)
	mk_sysconf_map_entry (_SC_SEM_VALUE_MAX),
#endif

#if defined (_SC_SHARED_MEMORY_OBJECTS)
	mk_sysconf_map_entry (_SC_SHARED_MEMORY_OBJECTS),
#endif

#if defined (_SC_SHELL)
	mk_sysconf_map_entry (_SC_SHELL),
#endif

#if defined (_SC_SHRT_MAX)
	mk_sysconf_map_entry (_SC_SHRT_MAX),
#endif

#if defined (_SC_SHRT_MIN)
	mk_sysconf_map_entry (_SC_SHRT_MIN),
#endif

#if defined (_SC_SIGNALS)
	mk_sysconf_map_entry (_SC_SIGNALS),
#endif

#if defined (_SC_SPAWN)
	mk_sysconf_map_entry (_SC_SPAWN),
#endif

#if defined (_SC_SPIN_LOCKS)
	mk_sysconf_map_entry (_SC_SPIN_LOCKS),
#endif

#if defined (_SC_SPORADIC_SERVER)
	mk_sysconf_map_entry (_SC_SPORADIC_SERVER),
#endif

#if defined (_SC_SYNCHRONIZED_IO)
	mk_sysconf_map_entry (_SC_SYNCHRONIZED_IO),
#endif

#if defined (_SC_THREAD_ATTR_STACKADDR)
	mk_sysconf_map_entry (_SC_THREAD_ATTR_STACKADDR),
#endif

#if defined (_SC_THREAD_ATTR_STACKSIZE)
	mk_sysconf_map_entry (_SC_THREAD_ATTR_STACKSIZE),
#endif

#if defined (_SC_THREAD_CPUTIME)
	mk_sysconf_map_entry (_SC_THREAD_CPUTIME),
#endif

#if defined (_SC_THREAD_PRIO_INHERIT)
	mk_sysconf_map_entry (_SC_THREAD_PRIO_INHERIT),
#endif

#if defined (_SC_THREAD_PRIO_PROTECT)
	mk_sysconf_map_entry (_SC_THREAD_PRIO_PROTECT),
#endif

#if defined (_SC_THREAD_PRIORITY_SCHEDULING)
	mk_sysconf_map_entry (_SC_THREAD_PRIORITY_SCHEDULING),
#endif

#if defined (_SC_THREAD_PROCESS_SHARED)
	mk_sysconf_map_entry (_SC_THREAD_PROCESS_SHARED),
#endif

#if defined (_SC_THREAD_ROBUST_PRIO_INHERIT)
	mk_sysconf_map_entry (_SC_THREAD_ROBUST_PRIO_INHERIT),
#endif

#if defined (_SC_THREAD_ROBUST_PRIO_PROTECT)
	mk_sysconf_map_entry (_SC_THREAD_ROBUST_PRIO_PROTECT),
#endif

#if defined (_SC_THREAD_SAFE_FUNCTIONS)
	mk_sysconf_map_entry (_SC_THREAD_SAFE_FUNCTIONS),
#endif

#if defined (_SC_THREAD_SPORADIC_SERVER)
	mk_sysconf_map_entry (_SC_THREAD_SPORADIC_SERVER),
#endif

#if defined (_SC_THREADS)
	mk_sysconf_map_entry (_SC_THREADS),
#endif

#if defined (_SC_TIMEOUTS)
	mk_sysconf_map_entry (_SC_TIMEOUTS),
#endif

#if defined (_SC_TIMERS)
	mk_sysconf_map_entry (_SC_TIMERS),
#endif

#if defined (_SC_TRACE)
	mk_sysconf_map_entry (_SC_TRACE),
#endif

#if defined (_SC_TRACE_EVENT_FILTER)
	mk_sysconf_map_entry (_SC_TRACE_EVENT_FILTER),
#endif

#if defined (_SC_TRACE_INHERIT)
	mk_sysconf_map_entry (_SC_TRACE_INHERIT),
#endif

#if defined (_SC_TRACE_LOG)
	mk_sysconf_map_entry (_SC_TRACE_LOG),
#endif

#if defined (_SC_TYPED_MEMORY_OBJECT)
	mk_sysconf_map_entry (_SC_TYPED_MEMORY_OBJECT),
#endif

#if defined (_SC_XBS5_ILP32_OFF32)
	mk_sysconf_map_entry (_SC_XBS5_ILP32_OFF32),
#endif

#if defined (_SC_XBS5_ILP32_OFFBIG)
	mk_sysconf_map_entry (_SC_XBS5_ILP32_OFFBIG),
#endif

#if defined (_SC_XBS5_LP64_OFF64)
	mk_sysconf_map_entry (_SC_XBS5_LP64_OFF64),
#endif

#if defined (_SC_XBS5_LPBIG_OFFBIG)
	mk_sysconf_map_entry (_SC_XBS5_LPBIG_OFFBIG),
#endif

#if defined (_SC_XOPEN_CRYPT)
	mk_sysconf_map_entry (_SC_XOPEN_CRYPT),
#endif

#if defined (_SC_XOPEN_ENH_I18N)
	mk_sysconf_map_entry (_SC_XOPEN_ENH_I18N),
#endif

#if defined (_SC_XOPEN_LEGACY)
	mk_sysconf_map_entry (_SC_XOPEN_LEGACY),
#endif

#if defined (_SC_XOPEN_REALTIME)
	mk_sysconf_map_entry (_SC_XOPEN_REALTIME),
#endif

#if defined (_SC_XOPEN_REALTIME_THREADS)
	mk_sysconf_map_entry (_SC_XOPEN_REALTIME_THREADS),
#endif

#if defined (_SC_XOPEN_SHM)
	mk_sysconf_map_entry (_SC_XOPEN_SHM),
#endif

#if defined (_SC_XOPEN_UNIX)
	mk_sysconf_map_entry (_SC_XOPEN_UNIX),
#endif

#if defined (_SC_XOPEN_XPG2)
	mk_sysconf_map_entry (_SC_XOPEN_XPG2),
#endif

#if defined (_SC_XOPEN_XPG3)
	mk_sysconf_map_entry (_SC_XOPEN_XPG3),
#endif

#if defined (_SC_XOPEN_XPG4)
	mk_sysconf_map_entry (_SC_XOPEN_XPG4),
#endif

#if defined (_SC_XOPEN_VERSION)
	mk_sysconf_map_entry (_SC_XOPEN_VERSION),
#endif

#if defined (_SC_XOPEN_XCU_VERSION)
	mk_sysconf_map_entry (_SC_XOPEN_XCU_VERSION),
#endif

	{ 0, NULL }
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

struct procenv_map thread_sched_policy_map[] = {
	mk_map_entry (SCHED_OTHER),
	mk_map_entry (SCHED_FIFO),
	mk_map_entry (SCHED_RR)
};

struct network_map {
	struct ifaddrs   ifaddr;
	char            *mac_address;

	struct network_map *next;
	struct network_map *prev;
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
	show ("  -a, --meta              : Display meta details.");
	show ("  -A, --arguments         : Display program arguments.");
	show ("  -b, --libs              : Display details of linked libraries.");
	show ("  -B, --libc              : Display standard library details.");
	show ("  -c, --cgroups           : Display cgroup details (Linux only).");
	show ("  -C, --cpu               : Display CPU and scheduler details.");
	show ("  --crumb-separator=<str> : Specify string '<str>' as alternate delimiter");
	show ("                            for crumb format output (default='%s').",
			get_crumb_separator ());
	show ("  -d, --compiler          : Display compiler details.");
	show ("  -e, --environment       : Display environment variables.");
	show ("  -E, --semaphores        : Display semaphore details.");
	show ("  --exec                  : Treat non-option arguments as program to execute.");
	show ("  -f, --fds               : Display file descriptor details.");
	show ("  -F, --namespaces        : Display namespace details.");
	show ("  --file=<file>           : Send output to <file> (implies --output=file).");
	show ("  --file-append           : Append to file specified by '--file='");
    show ("                            rather than overwriting");
	show ("  --format=<format>       : Specify output format. <format> can be one of:");
	show ("");
	show ("                            crumb    : ASCII 'breadcrumbs'");
	show ("                            json     : JSON output.");
	show ("                            text     : ASCII output (default).");
	show ("                            xml      : XML output.");
	show ("");
	show ("  -g, --sizeof            : Display sizes of data types in bytes.");
	show ("  -h, --help              : This help text.");
	show ("  -i, --misc              : Display miscellaneous details.");
	show ("  --indent                : Number of indent characters to use for each indent");
	show ("                            (default=%d).", get_indent_amount ());
	show ("  --indent-char=<c>       : Use character '<c>' for indenting");
	show ("                            (default='%s').", get_indent_char ());
	show ("  -j, --uname             : Display uname details.");
	show ("  -k, --clocks            : Display clock details.");
	show ("  -l, --limits            : Display limits.");
	show ("  -L, --locale            : Display locale details.");
	show ("  -m, --mounts            : Display mount details.");
	show ("  -M, --message-queues    : Display message queue details.");
	show ("  -n, --confstr           : Display confstr details.");
	show ("  -N, --network           : Display network details.");
	show ("  -o, --oom               : Display out-of-memory manager details (Linux only)");
	show ("  --output=<type>         : Send output to alternative location.");
	show ("                            <type> can be one of:");
	show ("");
	show ("                            file     : Send output to a file.");
	show ("                            stderr   : Write to standard error.");
	show ("                            stdout   : Write to standard output (default).");
	show ("                            syslog   : Write to the system log file.");
	show ("                            terminal : Write to terminal.");
	show ("");
	show ("  -p, --process           : Display process details.");
	show ("  -P, --platform          : Display platform details.");
	show ("  -q, --time              : Display time details.");
	show ("  -r, --ranges            : Display range of data types.");
	show ("  --separator=<str>       : Specify string '<str>' as alternate delimiter");
	show ("                            for text format output (default='%s').",
			get_text_separator ());
	show ("  -s, --signals           : Display signal details.");
	show ("  -S, --shared-memory     : Display shared memory details.");
	show ("  -t, --tty               : Display terminal details.");
	show ("  -T, --threads           : Display thread details.");
	show ("  -u, --stat              : Display stat details.");
	show ("  -U, --rusage            : Display rusage details.");
	show ("  -v, --version           : Display version details.");
	show ("  -w, --capabilities      : Display capability details (Linux only).");
	show ("  -x, --pathconf          : Display pathconf details.");
	show ("  -y, --sysconf           : Display sysconf details.");
	show ("  -Y, --memory            : Display memory details.");
	show ("  -z, --timezone          : Display timezone details.");
	show ("");
	show ("Notes:");
	show ("");
	show ("  - Options are considered in order, so '--output' should");
	show ("    precede any other option.");
	show ("  - If no display option is specified, all details are displayed.");
	show ("  - Only one display option may be specified.");
	show ("  - All values for '--indent-char' are literal except '\\t' which can be");
	show ("    used to specify a tab character. The same is true for '--separator'");
	show ("    and '--crumb-separator' but only if it is the first character specified.");
	show ("  - Specifying a visible indent-char is only (vaguely) meaningful");
	show ("    for text output.");
	show ("  - Any long option name may be shortened as long as it remains unique.");
	show ("  - The 'crumb' output format is designed for easy parsing: it displays");
	show ("    the data in a flattened format with each value on a separate line");
	show ("    preceded by all appropriate headings which are separated by the");
	show ("    current separator.");
	show ("");
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
 * (using clone's CLONE_SIGHAND+CLONE_VM flags). This is possible since
 * the child then shares the parents signal handlers, which inherantly
 * therefore provide access to the dispositions).
 **/
void
show_signals (void)
{
	int                        i;
	int                        rc;
	int                        blocked;
	int                        ignored;
	sigset_t                   old_sigset;
	struct sigaction           act;
	const struct procenv_map  *p;
	int                        max;

	container_open ("signals");

	/* Query blocked signals.
	 *
	 * arg 1 ('how') should be 0, but valgrind complains.
	 */
	if (sigprocmask (SIG_BLOCK, NULL, &old_sigset) < 0)
		die ("failed to query signal mask");

	/* first, count the number of entries in the platform-specific
	 * signal map.
	 */
	for (p = ops->signal_map, max = 1;
			p && p->name;
			p++, max++) {
		; /* NOP */
	}

	/* Note that we don't interate the signal map directly in case the
	 * entries are out of order. Instead, present the signals in numeric
	 * order.
	 */
	for (i = 1; i < max; i++) {
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
		if (! signal_name)
			continue;

		signal_desc = strsignal (i);

		object_open (false);

		section_open (signal_name);

		entry ("number", "%d", i);
		entry ("description", "'%s'",
				signal_desc ? signal_desc : UNKNOWN_STR);
		entry ("blocked", "%s", blocked ? YES_STR : NO_STR);
		entry ("ignored", "%s", ignored ? YES_STR : NO_STR);

		section_close ();

		object_close (false);
	}

	container_close ();
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
	struct procenv_map  *p;
	long                 value;

	header ("sysconf");

	for (p = sysconf_map; p && p->name; p++) {
		value = get_sysconf (p->num);
		if (value == -1) {
			entry (p->name, "%s", NA_STR);
		} else {
			entry (p->name, "%ld", value);
		}
	}

	footer ();
}

void
get_misc (void)
{
	misc.umask_value = umask (S_IWGRP|S_IWOTH);
	(void)umask (misc.umask_value);
	assert (getcwd (misc.cwd, sizeof (misc.cwd)));

	if (ops->get_user_misc)
		ops->get_user_misc (&user, &misc);
}

void
show_proc (void)
{
	const char *suid = NULL;
	const char *sgid = NULL;

	header ("process");

	entry ("process id (pid)", "%d", user.pid);

	entry ("parent process id (ppid)", "%d", user.ppid);
	entry ("session id (sid)", "%d (leader=%s)",
			user.sid,
			is_session_leader () ? YES_STR : NO_STR);

	entry ("name", "'%s'", user.proc_name);

	handle_proc_branch ();

	entry ("process group id", "%d (leader=%s)",
			user.pgroup,
			is_process_group_leader () ? YES_STR : NO_STR);

	entry ("foreground process group", "%d", user.fg_pgroup);

	entry ("terminal", "'%s'", user.ctrl_terminal);

	entry ("has controlling terminal", "%s",
			has_ctty () ? YES_STR : NO_STR);

	/* FIXME: Is it possible to detect if on console on hurd/minix? */
#if defined (PROCENV_PLATFORM_HURD)  || \
    defined (PROCENV_PLATFORM_MINIX) || \
    defined (PROCENV_PLATFORM_DARWIN)
	entry ("on console", "%s", UNKNOWN_STR);
#else
	entry ("on console", "%s",
			is_console (user.tty_fd) ? YES_STR : NO_STR);
#endif

	section_open ("user");

	entry ("real user id (uid)", "%d ('%s')",
			user.uid,
			get_user_name (user.uid));

	entry ("effective user id (euid)", "%d ('%s')",
			user.euid,
			get_user_name (user.euid));

	suid = get_user_name (user.suid);
	entry ("saved set-user-id (suid)", "%d (%s%s%s)",
			user.suid,
			suid ? "'" : "",
			suid ? suid : UNKNOWN_STR,
			suid ? "'" : "");

	section_close ();

	section_open ("group");

	entry ("real group id (gid)", "%d ('%s')",
			user.gid,
			get_group_name (user.gid));

	entry ("effective group id (egid)", "%d ('%s')",
			user.egid,
			get_group_name (user.egid));

	sgid = get_group_name (user.sgid);
	entry ("saved set-group-id (sgid)", "%d (%s%s%s)",
			user.sgid,
			sgid ? "'" : "",
			sgid ? sgid : UNKNOWN_STR,
			sgid ? "'" : "");

	section_close ();

	entry ("login name", "'%s'", user.login ? user.login : "");

	section_open ("passwd");

	entry ("name", "'%s'", user.passwd.pw_name);

#if ! defined (PROCENV_PLATFORM_ANDROID)
	/* No gecos on Android. In fact it doesn't actually use the
	 * passwd database, but meh.
	 */
	entry ("gecos", "'%s'", user.passwd.pw_gecos);
#endif

	entry ("dir", "'%s'", user.passwd.pw_dir);
	entry ("shell", "'%s'", user.passwd.pw_shell);

	section_close ();

#if ! defined (PROCENV_PLATFORM_MINIX)
	/* Calling getgroups(2) with Minix 3.3.0 always returns EINVAL :( */
	show_all_groups ();
#endif

	footer ();
}

void
show_priorities (void)
{
	section_open ("scheduler");

	if (ops->handle_scheduler_type)
		ops->handle_scheduler_type ();

	section_open ("priority");

	entry ("process", "%d", priority.process);
	entry ("process group", "%d", priority.pgrp);
	entry ("user", "%d", priority.user);

	section_close ();

	if (ops->show_io_priorities) {
		section_open ("I/O priority");
		ops->show_io_priorities ();
		section_close ();
	}

	section_close ();
}

void
show_platform (void)
{
	long kernel_bits = -1;
	long executable_bits;

	header ("platform");

	entry ("operating system", "%s", get_os ());
	entry ("architecture", "%s", get_arch ());

	if (ops->get_kernel_bits)
		kernel_bits = ops->get_kernel_bits ();

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

	if (ops->show_cpu)
		ops->show_cpu ();

	if (ops->show_cpu_affinities)
		ops->show_cpu_affinities ();

	show_priorities ();

	footer ();
}

void
show_memory (void)
{
	header ("memory");

	entry ("page size", "%d bytes", getpagesize ());

	if (ops->handle_numa_memory)
		ops->handle_numa_memory ();

	footer ();
}

void
show_fds (void)
{
	container_open ("file descriptors");

	if (ops->show_fds)
		ops->show_fds ();

	container_close ();
}

void
show_namespaces (void)
{
	container_open ("namespaces");

	if (ops->show_namespaces)
		ops->show_namespaces ();

	container_close ();
}

void
show_env (void)
{
	char    **env = environ;
	char    **copy;
	char     *name;
	char     *value;
	size_t    i;
	size_t    max;

	header ("environment");

	/* Calculate size of environment array */
	for (max=0; env[max]; max++)
		;

	copy = calloc (1+max, sizeof (char *));
	if (! copy)
		die ("failed to alloate storage");

	/* Copy the environ array since modification is not allowed */
	for (i=0; i < max; i++) {
		copy[i] = calloc (1, 1+strlen (env[i]));
		if (! copy[i])
			die ("failed to alloate storage");
		strcpy (copy[i], env[i]);
	}

	/* Terminate */
	copy[max] = NULL;

	/* Sort */
	env = copy;
	qsort (env, i, sizeof (env[0]), qsort_compar);

	/* Display */
	env = copy;
	while (env && *env) {
		name = *env;
		value = strchr (name, '=');
		assert (value);
		*value = '\0';
		value++;

		entry (name, "%s", value);
		env++;
	}

	/* Tidy up */
	for (i = 0; i < max; i++)
		free (copy[i]);

	free (copy);

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
#if defined (HAVE_GETRESUID) || defined (HAVE_GETRESGID)
	int            ret;
#endif

	user.pid  = getpid ();
	user.ppid = getppid ();

	if (ops->get_proc_name)
		ops->get_proc_name(&user);

#ifdef HAVE_GETRESUID
	ret = getresuid (&user.uid, &user.euid, &user.suid);
	assert (! ret);
#else
	user.uid  = getuid ();
	user.euid = geteuid ();
	/* no saved uid */
	user.suid = -1;
#endif

#ifdef HAVE_GETRESGID
	getresgid (&user.gid, &user.egid, &user.sgid);
	assert (! ret);
#else
	user.gid  = getgid ();
	user.egid = getegid ();
	/* no saved gid */
	user.sgid = -1;
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

#if defined (PROCENV_PLATFORM_ANDROID)
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

	errno = 0;
	pw = getpwuid (user.uid);
	if (! pw) {
		if (errno == 0) {
			die ("uid %d no longer exists", user.uid);
		}

		die ("cannot determine user details for uid %d: %s", user.uid, strerror (errno));
	}

	p = memcpy (&user.passwd, pw, sizeof (struct passwd));
	assert (p == (void *)&user.passwd);
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
	gid_t  *g;
	char  **group_names = NULL;
	size_t  bytes;

	bytes = size * sizeof (gid_t);

	groups = malloc (bytes);
	if (! groups)
		goto error;

	memset (groups, '\0', bytes);

	while (true) {
		ret = getgroups (size, groups);
		if (ret >= 0)
			break;

		size++;
		g = realloc (groups, (size * sizeof (gid_t)));
		if (! g) {
			free (groups);
			goto error;
		}

		groups = g;
	}

	size = ret;

	if (size == 0) {
		const char *group;

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
		appendf (&group_names[i], "'%s' (%d)",
				get_group_name (groups[i]), groups[i]);
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
save_locale (void)
{
	char  *value;

	assert (! saved_locale);

	/* save the existing value, but crucially also load the correct
	 * locale from the environment to enable UTF-8 functionality.
	 */
	value = setlocale (LC_ALL, "");
	if (! value) {
		/* Can't determine locale, so ignore */
		return;
	}

	saved_locale = strdup (value);
	if (! saved_locale)
		die ("failed to allocate space for locale");

	if (atexit (restore_locale))
		die ("failed to register exit handler");
}

void
restore_locale (void)
{
	if (! saved_locale) {
		/* Nothing to do */
		return;
	}

	(void)setlocale (LC_ALL, saved_locale);

	free (saved_locale);
}

void
init (void)
{
	assert (ops);

	if (ops->init)
		ops->init ();

	save_locale ();

	handle_indent_char ();

	/* required to allow for more graceful handling of prctl(2)
	 * options that were introduced in kernel version 'x.y'.
	 */
	get_uname ();

	get_user_info ();
	get_misc ();
	get_priorities ();

	if (ops->get_io_priorities)
		ops->get_io_priorities (&priority_io);
}

void
cleanup (void)
{
	common_assert ();
	assert (doc);

	close (user.tty_fd);

	output_finalise ();

	pstring_free (doc);

	if (ops->cleanup)
		ops->cleanup ();
}

void
show_meta (void)
{
	const char *build_type;

#if defined (PROCENV_REPRODUCIBLE_BUILD)
	build_type = BUILD_TYPE_REPRODUCIBLE_STR;
#else
	build_type = BUILD_TYPE_STD_STR;
#endif

	header ("meta");

	entry ("version", "%s", PACKAGE_VERSION);

	entry ("package", "%s", PACKAGE_STRING);
	entry ("mode", "%s%s",
			user.euid ? _(NON_STR) "-" : "",
			PRIVILEGED_STR);

	entry ("build-type", "%s", build_type);

	section_open ("driver");
	entry ("name", ops->driver.name);
	entry ("file", ops->driver.file);
	section_close ();

	entry ("format-type", "%s", get_output_format_name ());
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

		object_open (false);

		entry (buffer, "%s", argvp[i]);

		object_close (false);

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

/* Dump out data in alphabetical fashion */
void
dump (void)
{
	master_header (&doc);

	show_meta ();
	show_arguments ();

	show_capabilities ();
	show_cgroups ();
	show_clocks ();
	show_compiler ();
	show_confstrs ();
	show_cpu ();
	show_env ();
	show_fds ();
	show_libc ();
	show_libs ();
	show_rlimits ();
	show_locale ();
	show_memory ();
	show_msg_queues ();
	show_misc ();
	show_mounts (SHOW_ALL);
	show_namespaces ();
	show_network ();
	show_oom ();
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

void
decode_if_flags (unsigned int flags)
{
	const struct procenv_map *p;

	for (p = ops->if_flag_map; p && p->name; p++) {
		if (flags & p->num) {
			object_open (false);
			entry (p->name, "0x%x", p->num);
			object_close (false);
		}
	}
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

/*
 *
 * Returns: IEEE-802 format MAC address, or NULL on error.
 */
#if !defined (PROCENV_PLATFORM_MINIX)
char *
get_mac_address (const struct ifaddrs *ifaddr)
{
	char          *data = NULL;
	char          *mac_address = NULL;
	int            i;
	int            valid = false;
#if defined (PROCENV_PLATFORM_BSD) || defined (PROCENV_PLATFORM_DARWIN)
	struct sockaddr_dl *link_layer;
#else
	struct ifreq   ifr;
	int            sock = -1;
#endif

#if defined (PROCENV_PLATFORM_LINUX) || defined (PROCENV_PLATFORM_HURD) || defined (PROCENV_PLATFORM_GENERIC)
	unsigned long  request = SIOCGIFHWADDR;
#endif

	assert (ifaddr);

#if defined (PROCENV_PLATFORM_BSD) || defined (PROCENV_PLATFORM_DARWIN)
	link_layer = (struct sockaddr_dl *)ifaddr->ifa_addr;
#else

	/* We need to create a socket to query an interfaces MAC
	 * address.
	 */
	sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (sock < 0)
		return NULL;

	memset (&ifr, 0, sizeof (struct ifreq));
	strncpy (ifr.ifr_name, ifaddr->ifa_name, IFNAMSIZ-1);

	if (ioctl (sock, request, &ifr) < 0)
		goto out;
#endif

#if defined (PROCENV_PLATFORM_BSD) || defined (PROCENV_PLATFORM_DARWIN)
	data = LLADDR (link_layer);
#else
	data = (char *)ifr.ifr_hwaddr.sa_data;
#endif

	if (data) {
		for (i = 0; i < 6; i++) {
			if (data[i]) {
				valid = true;
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

#if defined (PROCENV_PLATFORM_BSD) || defined (PROCENV_PLATFORM_DARWIN)
	/* NOP */
#else
	close (sock);
#endif
	return mac_address;
}
#endif /* PROCENV_PLATFORM_MINIX */


#if defined (PROCENV_PLATFORM_LINUX)
const char *
get_personality_name (unsigned int domain)
{
	const struct procenv_map *m;

	for (m = ops->personality_map; m && m->name; m++) {
		if (m->num == (domain & PER_MASK))
			return m->name;
	}

	return NULL;
}

char *
get_personality_flags (unsigned int flags)
{
	const struct procenv_map  *m;
	char                       *list = NULL;
	int                        first = true;

	for (m = ops->personality_flag_map; m && m->name; m++) {
		if (flags & m->num) {
			appendf (&list, "%s%s",
					first ? "" : ", ",
					m->name);
			first = false;
		}
	}

	return list;
}

#endif /* PROCENV_PLATFORM_LINUX */

void
show_mounts (ShowMountType what)
{
	common_assert ();

	header (what == SHOW_PATHCONF ? "pathconf" : "mounts");

	if (ops->show_mounts)
		ops->show_mounts (what);

	footer ();
}

const char *
get_net_family_name (sa_family_t family)
{
	switch (family) {
#if defined (PROCENV_PLATFORM_LINUX)
	case AF_PACKET:
		return "AF_PACKET";
		break;
#endif

#if defined (PROCENV_PLATFORM_BSD)
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
	unsigned short       ext_flags = 0;
	sa_family_t          family;
	char                 address[NI_MAXHOST];
	int                  mtu = 0;

	common_assert ();
	assert (ifa);

	family = ifa->ifa_addr->sa_family;

	assert (ifa->ifa_name);
	section_open (ifa->ifa_name);

	entry ("family", "%s (0x%x)", get_net_family_name (family), family);

	/*******************************/

	section_open ("flags");

	entry ("value", "0x%x", ifa->ifa_flags);

	container_open ("fields");

	decode_if_flags (ifa->ifa_flags);

	container_close ();

	section_close ();

	/*******************************/

	section_open ("extended flags");

	entry ("value", "0x%x", ext_flags);

	container_open ("fields");

	if (ops->show_extended_if_flags)
		ops->show_extended_if_flags (ifa->ifa_name, &ext_flags);

	container_close ();

	section_close ();

	/*******************************/

	if (ops->get_mtu)
		mtu = ops->get_mtu (ifa);

#if defined (PROCENV_PLATFORM_HURD)
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

#if !defined (PROCENV_PLATFORM_HURD)
#if defined (IFF_BROADCAST)
	if (family != PROCENV_LINK_LEVEL_FAMILY) {
		if ((ifa->ifa_flags & IFF_BROADCAST) && ifa->ifa_broadaddr) {
			get_network_address (ifa->ifa_broadaddr, family, address);

			entry ("broadcast", "%s", ifa->ifa_broadaddr ? address : NA_STR);
		}
	} else {
#endif
#endif
		entry ("broadcast", "%s", NA_STR);
#if !defined (PROCENV_PLATFORM_HURD)
#if defined (IFF_BROADCAST)
	}
#endif
#endif

#if defined (IFF_POINTOPOINT)
	if (ifa->ifa_flags & IFF_POINTOPOINT && ifa->ifa_dstaddr) {

		get_network_address (ifa->ifa_dstaddr, family, address);

		entry ("point-to-point", "%s", address);
	}
#endif

	section_close ();
}

void
get_priorities (void)
{
	priority.process = getpriority (PRIO_PROCESS, 0);
	priority.pgrp    = getpriority (PRIO_PGRP   , 0);
	priority.user    = getpriority (PRIO_USER   , 0);
}

void
handle_proc_branch (void)
{
	common_assert ();

	if (ops->handle_proc_branch)
		ops->handle_proc_branch ();
}

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

	if (ops->get_tty_locked_status)
		ops->get_tty_locked_status (&lock_status);

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

#if defined (IUCLC)
	show_const_tty (tty, c_iflag, IUCLC, lock_status);
#endif
	show_const_tty (tty, c_iflag, IXON, lock_status);
	show_const_tty (tty, c_iflag, IXANY, lock_status);
	show_const_tty (tty, c_iflag, IXOFF, lock_status);
	show_const_tty (tty, c_iflag, IMAXBEL, lock_status);

#if defined (IUTF8)
	show_const_tty (tty, c_iflag, IUTF8, lock_status);
#endif

	section_close ();

	/*****************************************/
	section_open ("c_oflag (output)");

	entry ("c_oflag", "0x%x", tty.c_oflag);

	show_const_tty (tty, c_oflag, OPOST, lock_status);

#if defined (OLCUC)
	show_const_tty (tty, c_oflag, OLCUC, lock_status);
#endif
	show_const_tty (tty, c_oflag, ONLCR, lock_status);
	show_const_tty (tty, c_oflag, OCRNL, lock_status);
	show_const_tty (tty, c_oflag, ONOCR, lock_status);
	show_const_tty (tty, c_oflag, ONLRET, lock_status);
#if defined (OFILL)
	show_const_tty (tty, c_oflag, OFILL, lock_status);
#endif

#if defined (OFDEL)
	show_const_tty (tty, c_oflag, OFDEL, lock_status);
#endif

#if defined (NLDLY)
	show_const_tty (tty, c_oflag, NLDLY, lock_status);
#endif

#if defined (CRDLY)
	show_const_tty (tty, c_oflag, CRDLY, lock_status);
#endif

#if defined (TABDLY)
	show_const_tty (tty, c_oflag, TABDLY, lock_status);
#endif

#if defined (BSDLY)
	show_const_tty (tty, c_oflag, BSDLY, lock_status);
#endif

#if defined (VTDLY)
	show_const_tty (tty, c_oflag, VTDLY, lock_status);
#endif

#if defined (FFDLY)
	show_const_tty (tty, c_oflag, FFDLY, lock_status);
#endif

	section_close ();

	/*****************************************/
	section_open ("c_cflag (control)");

	entry ("value", "0x%x", tty.c_cflag);

#if defined (CBAUDEX)
	show_const_tty (tty, c_cflag, CBAUDEX, lock_status);
#endif

	show_const_tty (tty, c_cflag, CSIZE, lock_status);
	show_const_tty (tty, c_cflag, CSTOPB, lock_status);
	show_const_tty (tty, c_cflag, CREAD, lock_status);
	show_const_tty (tty, c_cflag, PARENB, lock_status);
	show_const_tty (tty, c_cflag, PARODD, lock_status);
	show_const_tty (tty, c_cflag, HUPCL, lock_status);
	show_const_tty (tty, c_cflag, CLOCAL, lock_status);

#ifdef CIBAUD
	show_const_tty (tty, c_cflag, CIBAUD, lock_status);
#endif

#ifdef CMSPAR
	show_const_tty (tty, c_cflag, CMSPAR, lock_status);
#endif

	show_const_tty (tty, c_cflag, CRTSCTS, lock_status);

	section_close ();

	/*****************************************/
	section_open ("c_lflag (local)");

	entry ("value", "0x%x", tty.c_lflag);

	show_const_tty (tty, c_lflag, ISIG, lock_status);

#if defined (XCASE)
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

#if defined (VSWTC)
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

	const struct procenv_map *p;

	for (p = ops->signal_map; p && p->name; p++) {
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
const char *
get_os (void)
{
#ifdef _AIX
	return "AIX";
#endif

#ifdef __ANDROID__
	return "Android";
#endif

#if (defined (__MACH__) && defined (__GNU__)) || defined (__gnu_hurd__)
	return "GNU (Hurd)";
#endif

#if defined (__hpux) || defined (hpux) || defined (_hpux)
	return "HP-UX";
#endif

#if defined (OS400) || defined (__OS400__)
	return "iSeries (OS/400)";
#endif

#ifdef PROCENV_PLATFORM_LINUX
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

#ifdef __FreeBSD__
	return "FreeBSD";
#endif

#ifdef __NetBSD__
	return "NetBSD";
#endif

#ifdef __OpenBSD__
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

#if defined (__APPLE__)
	return "Apple/Darwin";
#endif

#ifndef __COVERITY__
	return UNKNOWN_STR;
#endif
}

/**
 * get_arch:
 *
 * Returns: static string representing best guess
 * at architecture.
 **/
const char *
get_arch (void)
{

#ifdef __arm__
#ifdef __ARM_PCS_VFP
	return "ARMhf";
#endif
#ifdef __ARMEL__
	return "ARMEL";
#endif
	return "ARM";
#endif

	/* not arm apparently! :) */
#ifdef __aarch64__
	return "ARM64/AARCH64";
#endif

#if defined (__OR1K__) || defined (__or1k__)
	return "OpenRISC";
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

#if defined (__powerpc64__) || defined (__ppc64__)

#if defined (_LITTLE_ENDIAN)
	return "PPC64LE";
#endif
	return "PPC64/PowerPC64";
#endif

#ifdef __powerpc__
#if defined (__SPE__) && __SIZEOF_POINTER__ == 4
	return "PPCspe";
#endif
	return "PowerPC";
#endif

#if defined (__sparc64__) || defined (__sparc_v9__)
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

#ifdef __sh__
	return "SuperH";
#endif

#if defined (__s390__) || defined (__zarch__) || defined (__SYSC_ZARCH__) || defined (__THW_370__)
	return "SystemZ";
#endif

#if defined (__x86_64__) || defined (__x86_64) || defined (__amd64)
	return "x64/AMD64";
#endif

	return UNKNOWN_STR;
}

#if ! defined (PROCENV_PLATFORM_ANDROID) && ! defined (PROCENV_PLATFORM_DARWIN)
static int
libs_callback (struct dl_phdr_info *info, size_t size, void *data)
{
	const char *name;
	const char *path;

	assert (info);

	if (! info->dlpi_name || ! *info->dlpi_name)
		return 0;

	path = info->dlpi_name;
	assert (path);

	name = strrchr (path, '/');

	if (name) {
		/* Jump over slash */
		name++;
	} else {
		/* BSD libraries don't show the path */
		name = path;
	}

	object_open (false);

	section_open (name);

	entry ("path", "%s", path);
	entry ("address", "%p", (void *)info->dlpi_addr);

	section_close ();

	object_close (false);

	return 0;
}
#endif

void
show_libs (void)
{
	common_assert ();

	container_open ("libraries");

#if ! defined (PROCENV_PLATFORM_ANDROID) && ! defined (PROCENV_PLATFORM_DARWIN)
	dl_iterate_phdr (libs_callback, NULL);
#endif

	container_close ();
}

void
show_clocks (void)
{
	header ("clocks");

	if (ops->show_clocks)
		ops->show_clocks();

	footer ();
}

void
show_rlimits (void)
{
	header ("limits");

	if (ops->show_rlimits)
		ops->show_rlimits ();

	footer ();
}

void
show_confstrs (void)
{
	header ("confstr");

	if (ops->show_confstrs)
		ops->show_confstrs();

	footer ();
}

void
show_timezone (void)
{
	header ("timezone");

	if (ops->show_timezone)
		ops->show_timezone ();

	footer ();
}

void
show_sizeof (void)
{
	header ("sizeof");

	entry ("bits/byte (CHAR_BIT)", "%d", CHAR_BIT);

	show_sizeof_type (blkcnt_t);
	show_sizeof_type (blksize_t);
	show_sizeof_type (char);

#if !defined (PROCENV_PLATFORM_DARWIN)
	show_sizeof_type (clockid_t);
#endif

	show_sizeof_type (clock_t);
	show_sizeof_type (dev_t);
	show_sizeof_type (div_t);
	show_sizeof_type (double);
	show_sizeof_type (fenv_t);
	show_sizeof_type (fexcept_t);
	show_sizeof_type (float);
	show_sizeof_type (fpos_t);
	show_sizeof_type (fsblkcnt_t);
	show_sizeof_type (fsfilcnt_t);
	show_sizeof_type (gid_t);
	show_sizeof_type (id_t);
	show_sizeof_type (imaxdiv_t);
	show_sizeof_type (ino_t);
	show_sizeof_type (int);
	show_sizeof_type (int16_t);
	show_sizeof_type (int32_t);
	show_sizeof_type (int64_t);
	show_sizeof_type (int8_t);
	show_sizeof_type (int_fast16_t);
	show_sizeof_type (int_fast32_t);
	show_sizeof_type (int_fast64_t);
	show_sizeof_type (int_fast8_t);
	show_sizeof_type (int_least16_t);
	show_sizeof_type (int_least32_t);
	show_sizeof_type (int_least64_t);
	show_sizeof_type (int_least8_t);
	show_sizeof_type (intmax_t);
	show_sizeof_type (intptr_t);
	show_sizeof_type (key_t);
	show_sizeof_type (ldiv_t);
	show_sizeof_type (lldiv_t);
	show_sizeof_type (long double);
	show_sizeof_type (long int);
	show_sizeof_type (long long int);
	show_sizeof_type (mbstate_t);
	show_sizeof_type (mode_t);
	show_sizeof_type (mode_t);
	show_sizeof_type (nlink_t);
	show_sizeof_type (off_t);
	show_sizeof_type (pid_t);
	show_sizeof_type (pthread_attr_t);

#if !defined (PROCENV_PLATFORM_MINIX) && !defined (PROCENV_PLATFORM_DARWIN)
	show_sizeof_type (pthread_barrierattr_t);
	show_sizeof_type (pthread_barrier_t);
#endif

	show_sizeof_type (pthread_condattr_t);
	show_sizeof_type (pthread_cond_t);
	show_sizeof_type (pthread_key_t);
	show_sizeof_type (pthread_mutexattr_t);
	show_sizeof_type (pthread_mutex_t);
	show_sizeof_type (pthread_once_t);
	show_sizeof_type (pthread_rwlockattr_t);
	show_sizeof_type (pthread_rwlock_t);

#if !defined (PROCENV_PLATFORM_MINIX) && !defined (PROCENV_PLATFORM_DARWIN)
	show_sizeof_type (pthread_spinlock_t);
#endif

	show_sizeof_type (pthread_t);
	show_sizeof_type (ptrdiff_t);
	show_sizeof_type (rlim_t);
	show_sizeof_type (short int);
	show_sizeof_type (sig_atomic_t);
	show_sizeof_type (size_t);
	show_sizeof_type (ssize_t);
	show_sizeof_type (suseconds_t);

#if !defined (PROCENV_PLATFORM_DARWIN)
	show_sizeof_type (timer_t);
#endif

	show_sizeof_type (time_t);
	show_sizeof_type (uid_t);
	show_sizeof_type (uint16_t);
	show_sizeof_type (uint32_t);
	show_sizeof_type (uint64_t);
	show_sizeof_type (uint8_t);
	show_sizeof_type (uint_fast16_t);
	show_sizeof_type (uint_fast32_t);
	show_sizeof_type (uint_fast64_t);
	show_sizeof_type (uint_fast8_t);
	show_sizeof_type (uint_least16_t);
	show_sizeof_type (uint_least32_t);
	show_sizeof_type (uint_least64_t);
	show_sizeof_type (uint_least8_t);
	show_sizeof_type (uintmax_t);
	show_sizeof_type (uintptr_t);
	show_sizeof_type (useconds_t);
	show_sizeof_type (void *);
	show_sizeof_type (wchar_t);
	show_sizeof_type (wint_t);

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
	entry ("symbolic", "%u to %s", 0, "UCHAR_MAX");

	entry ("scientific", "%e to %e", (double)0, (double)UCHAR_MAX);
	entry ("hex", "0x%.*x to 0x%.*x",
			type_hex_width (char), 0,
			type_hex_width (char), UCHAR_MAX);

	section_close ();

	section_open ("signed");
	entry ("decimal", "%d to %d", SCHAR_MIN, SCHAR_MAX);
	entry ("symbolic", "%s to %s", "SCHAR_MIN", "SCHAR_MAX");
	section_close ();

	section_close ();

	/******************************/
	section_open ("short int");

	show_size (short int);
	section_open ("unsigned");
	entry ("decimal", "%u to %u", 0, USHRT_MAX);
	entry ("symbolic", "%u to %s", 0, "USHRT_MAX");
	entry ("scientific", "%e to %e", (double)0, (double)USHRT_MAX);
	entry ("hex", "0x%.*x to 0x%.*x",
			type_hex_width (short int), 0,
			type_hex_width (short int), USHRT_MAX);
	section_close ();

	section_open ("signed");
	entry ("decimal", "%d to %d", SHRT_MIN, SHRT_MAX);
	entry ("symbolic", "%s to %s", "SHRT_MIN", "SHRT_MAX");
	section_close ();

	section_close ();

	/******************************/
	section_open ("int");

	show_size (int);
	section_open ("unsigned");
	entry ("decimal", "%u to %u", 0, UINT_MAX);
	entry ("symbolic", "%u to %s", 0, "UINT_MAX");
	entry ("scientific", "%e to %e", (double)0, (double)UINT_MAX);
	entry ("hex", "0x%.*x to 0x%.*x",
			type_hex_width (int), 0,
			type_hex_width (int), UINT_MAX);
	section_close ();

	section_open ("signed");
	entry ("decimal", "%d to %d", INT_MIN, INT_MAX);
	entry ("symbolic", "%s to %s", "INT_MIN", "INT_MAX");
	section_close ();

	section_close ();

	/******************************/
	section_open ("long int");

	show_size (long int);
	section_open ("unsigned");
	entry ("decimal", "%u to %u", 0, ULONG_MAX);
	entry ("symbolic", "%u to %s", 0, "ULONG_MAX");
	entry ("scientific", "%e to %e", (double)0, (double)ULONG_MAX);
	entry ("hex", "0x%.*x to 0x%.*x",
			type_hex_width (long int), 0L,
			type_hex_width (long int), ULONG_MAX);
	section_close ();

	section_open ("signed");
	entry ("decimal", "%ld to %ld", LONG_MIN, LONG_MAX);
	entry ("symbolic", "%s to %s", "LONG_MIN", "LONG_MAX");
	section_close ();

	section_close ();

	/******************************/
	section_open ("long long int");

	show_size (long long int);
	section_open ("unsigned");
	entry ("decimal", "%llu to %llu", 0, ULLONG_MAX);
	entry ("symbolic", "%u to %s", 0, "ULLONG_MAX");
	entry ("scientific", "%e to %e", (double)0, (double)ULLONG_MAX);
	entry ("hex", "0x%.*llx to 0x%.*llx",
			type_hex_width (long long int), 0LL,
			type_hex_width (long long int), ULLONG_MAX);
	section_close ();

	section_open ("signed");
	entry ("decimal", "%lld to %lld", LLONG_MIN, LLONG_MAX);
	entry ("symbolic", "%s to %s", "LLONG_MIN", "LLONG_MAX");
	section_close ();

	section_close ();

	/******************************/
	section_open ("float");
	show_size (float);
	entry ("signed", "%e to %e", FLT_MIN, FLT_MAX);
	entry ("symbolic", "%s to %s", "FLT_MIN", "FLT_MAX");
	section_close ();

	/******************************/
	section_open ("double");
	show_size (double);
	entry ("signed", "%le to %le", DBL_MIN, DBL_MAX);
	entry ("symbolic", "%s to %s", "DBL_MIN", "DBL_MAX");
	section_close ();

	/******************************/
	section_open ("long double");
	show_size (long double);
	entry ("signed", "%Le to %Le", LDBL_MIN, LDBL_MAX);
	entry ("symbolic", "%s to %s", "LDBL_MIN", "LDBL_MAX");
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

#if defined (PROCENV_REPRODUCIBLE_BUILD)
	entry ("compile date (__DATE__)", "%s", SUPPRESSED_STR);
	entry ("compile time (__TIME__)", "%s", SUPPRESSED_STR);

#ifdef __TIMESTAMP__
	entry ("timestamp (__TIMESTAMP__)", "%s", SUPPRESSED_STR);
#endif

#else
	entry ("compile date (__DATE__)", "%s", __DATE__);
	entry ("compile time (__TIME__)", "%s", __TIME__);

#ifdef __TIMESTAMP__
	entry ("timestamp (__TIMESTAMP__)", "%s", __TIMESTAMP__);
#endif
#endif

	entry ("translation unit (__FILE__)", "%s", __FILE__);
	entry ("base file (__BASE_FILE__)", "%s", __BASE_FILE__);

	section_open ("feature test macros");

#ifdef _ATFILE_SOURCE
	entry ("_ATFILE_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_ATFILE_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _BSD_SOURCE
	entry ("_BSD_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_BSD_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _DEFAULT_SOURCE
	entry ("_DEFAULT_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_DEFAULT_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _FILE_OFFSET_BITS
	entry ("_FILE_OFFSET_BITS", "%lu", _FILE_OFFSET_BITS);
#else
	entry ("_FILE_OFFSET_BITS", "%s", NOT_DEFINED_STR);
#endif

#ifdef _FORTIFY_SOURCE
	entry ("_FORTIFY_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_FORTIFY_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _GNU_SOURCE
	entry ("_GNU_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_GNU_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _ISOC11_SOURCE
	entry ("_ISOC11_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_ISOC11_SOURCE", "%s", NOT_DEFINED_STR);
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

#ifdef _LARGEFILE64_SOURCE
	entry ("_LARGEFILE64_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_LARGEFILE64_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _LARGEFILE_SOURCE
	entry ("_LARGEFILE_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_LARGEFILE_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _POSIX_C_SOURCE
	entry ("_POSIX_C_SOURCE", "%lu", _POSIX_C_SOURCE);
#else
	entry ("_POSIX_C_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#if defined (_POSIX_RAW_SOCKETS)
	entry ("_POSIX_RAW_SOCKETS", "%s", DEFINED_STR),
#else
	      entry ("_POSIX_RAW_SOCKETS", "%s", NOT_DEFINED_STR),
#endif

#ifdef _POSIX_SOURCE
	      entry ("_POSIX_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_POSIX_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _REENTRANT
	entry ("_REENTRANT", "%s", DEFINED_STR);
#else
	entry ("_REENTRANT", "%s", NOT_DEFINED_STR);
#endif

#ifdef __STDC_VERSION__
	entry ("__STDC_VERSION__", "%lu", __STDC_VERSION__);
#else
	entry ("__STDC_VERSION__", "%s", NOT_DEFINED_STR);
#endif

#ifdef __STRICT_ANSI__
	entry ("__STRICT_ANSI__", "%s", DEFINED_STR);
#else
	entry ("__STRICT_ANSI__", "%s", NOT_DEFINED_STR);
#endif

#ifdef __STDC_WANT_IEC_60559_BFP_EXT__
	entry ("__STDC_WANT_IEC_60559_BFP_EXT__", "%lu", __STDC_WANT_IEC_60559_BFP_EXT__);
#else
	entry ("__STDC_WANT_IEC_60559_BFP_EXT__", "%s", NOT_DEFINED_STR);
#endif

#ifdef __STDC_WANT_IEC_60559_FUNCS_EXT__
	entry ("__STDC_WANT_IEC_60559_FUNCS_EXT__", "%lu", __STDC_WANT_IEC_60559_FUNCS_EXT__);
#else
	entry ("__STDC_WANT_IEC_60559_FUNCS_EXT__", "%s", NOT_DEFINED_STR);
#endif

#ifdef __STDC_WANT_LIB_EXT2__
	entry ("__STDC_WANT_LIB_EXT2__", "%lu", __STDC_WANT_LIB_EXT2__);
#else
	entry ("__STDC_WANT_LIB_EXT2__", "%s", NOT_DEFINED_STR);
#endif

#ifdef _SVID_SOURCE
	entry ("_SVID_SOURCE", "%s", DEFINED_STR);
#else
	entry ("_SVID_SOURCE", "%s", NOT_DEFINED_STR);
#endif

#ifdef _THREAD_SAFE
	entry ("_THREAD_SAFE", "%s", DEFINED_STR);
#else
	entry ("_THREAD_SAFE", "%s", NOT_DEFINED_STR);
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

	section_close ();

	footer ();
}

void
show_libc (void)
{
	char  *name = NULL;
	long   version = -1;
	long   minor = -1;

#if defined (__GLIBC__)
	name = "GNU libc (glibc)";
	version = __GLIBC__;
#ifdef __GLIBC_MINOR__
	minor = __GLIBC_MINOR__;
#endif
#endif

#if defined (__UCLIBC__)
	name = "uClibc";
#ifdef __UCLIBC_MAJOR__
	version = __UCLIBC_MAJOR__;
#endif
#ifdef __UCLIBC_MINOR__
	minor = __UCLIBC_MINOR__;
#endif
#endif

#if defined (__KLIBC__)
	name = "klibc";
	version = __KLIBC__;
#ifdef __KLIBC_MINOR__
	minor = __KLIBC_MINOR__;
#endif
#endif

#if defined (__dietlibc__)
	name = "dietlibc";
#endif

#if defined (__BIONIC__)
	name = "bionic";
#endif

	header ("libc");

	entry ("name", "%s", name ? name : UNKNOWN_STR);

	if (version >= 0) {
		entry ("version", "%lu", version);
	} else {
		entry ("version", "%s", UNKNOWN_STR);
	}

	if (minor >= 0) {
		entry ("minor", "%lu", minor);
	} else {
		entry ("minor", "%s", UNKNOWN_STR);
	}

	footer ();
}

void
show_time (void)
{
	char              formatted_time[CTIME_BUFFER];
	struct timespec   ts;
	struct tm        *tm;

	if (! ops->get_time)
		return;

	if (ops->get_time (&ts))
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

#if defined (_GNU_SOURCE) && defined (PROCENV_PLATFORM_LINUX)
	entry ("domainname", "%s", uts.domainname[0] ? uts.domainname : UNKNOWN_STR);
#endif

	footer ();
}

void
show_cgroups (void)
{
	header ("cgroups");

	if (ops->show_cgroups)
		ops->show_cgroups ();

	footer ();
}

void
show_oom (void)
{
	header ("oom");

	if (ops->show_oom)
		ops->show_oom ();

	footer ();
}

void
show_capabilities (void)
{
	header ("capabilities");

	if (ops->show_capabilities)
		ops->show_capabilities();

	footer ();
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
		set_output_value (e);
	}

	e = getenv (PROCENV_FILE_ENV);
	if (e && *e) {
		set_output_file (e);
		set_output_value_raw (OUTPUT_FILE);
	}

	e = getenv (PROCENV_FILE_APPEND_ENV);
	if (e && *e) {
		set_output_file_append ();
	}

	e = getenv (PROCENV_FORMAT_ENV);
	if (e && *e) {
		set_output_format (e);
	}

	e = getenv (PROCENV_INDENT_ENV);
	if (e && *e) {
		set_indent_amount (atoi (e));
	}

	e = getenv (PROCENV_INDENT_CHAR_ENV);
	if (e && *e) {
		/* Special character handling */
		if (! strcmp (e, "\\t"))
			set_indent_char ("\t");
		else
			set_indent_char (e);
	}

	e = getenv (PROCENV_SEPARATOR_ENV);
	if (e && *e) {
		set_text_separator (e);
	}

	e = getenv (PROCENV_CRUMB_SEPARATOR_ENV);
	if (e && *e) {
		if (! strcmp (e, "\\t")) {
			set_crumb_separator ("\t");
		} else {
			set_crumb_separator (e);
		}
	}

	e = getenv (PROCENV_EXEC_ENV);
	if (e && *e) {
		char *tmp;

		string = strdup (e);
		if (! string) {
			die ("failed to copy environment string");
		}

		if (! *string) {
			die ("invalid command");
		}

		/* there must be atleast an no-arg command to run */
		count = 1;

		/* establish number of fields required for program arguments */
		for (tmp = string; tmp && *tmp; ) {
			tmp = index (tmp, ' ');
			if (tmp) {
				/* jump over matched char */
				tmp++;
				count++;
			}
		}

		/* allocate space for exec arguments.
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

		reexec = true;

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
		return false;
	}

	*_major = major (st.st_dev);
	*_minor = minor (st.st_dev);

	return true;
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

#if defined (PROCENV_PLATFORM_ANDROID)
	ret = 0;
	scope = pthread_attr_getscope (&attr);
#else
	ret = pthread_attr_getscope (&attr, &scope);
#endif
	entry ("scope", "%s",
			ret != 0 ? UNKNOWN_STR :
			scope == PTHREAD_SCOPE_SYSTEM ? "PTHREAD_SCOPE_SYSTEM"
			: "PTHREAD_SCOPE_PROCESS");

#if defined (PROCENV_PLATFORM_MINIX)
	{
		int size;
		ret = pthread_attr_getguardsize (&attr, &size);
		guard_size = size;
	}
#else
	ret = pthread_attr_getguardsize (&attr, &guard_size);
#endif

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

#ifdef PROCENV_PLATFORM_ANDROID
	section_close ();
#else
	ret = pthread_attr_getinheritsched (&attr, &inherit_sched);
	entry ("inherit-scheduler attribute", "%s",
			ret != 0 ? UNKNOWN_STR :
			inherit_sched == PTHREAD_INHERIT_SCHED
			?  "PTHREAD_INHERIT_SCHED"
			: "PTHREAD_EXPLICIT_SCHED");

	section_close ();

#if !defined (PROCENV_PLATFORM_NETBSD)
	entry ("concurrency", "%d", pthread_getconcurrency ());
#endif

#endif

	footer ();
}

const char *
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
	char     data_model[16];
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
	int  option;
	int  long_index;
	int  done = false;

	struct option long_options[] = {
		{"meta"            , no_argument       , NULL, 'a'},
		{"arguments"       , no_argument       , NULL, 'A'},
		{"libs"            , no_argument       , NULL, 'b'},
		{"libc"            , no_argument       , NULL, 'B'},
		{"cgroups"         , no_argument       , NULL, 'c'},
		{"cpu"             , no_argument       , NULL, 'C'},
		{"compiler"        , no_argument       , NULL, 'd'},
		{"crumb-separator" , required_argument , NULL,  0 },
		{"environment"     , no_argument       , NULL, 'e'},
		{"semaphores"      , no_argument       , NULL, 'E'},
		{"fds"             , no_argument       , NULL, 'f'},
		{"namespaces"      , no_argument       , NULL, 'F'},
		{"sizeof"          , no_argument       , NULL, 'g'},
		{"help"            , no_argument       , NULL, 'h'},
		{"misc"            , no_argument       , NULL, 'i'},
		{"uname"           , no_argument       , NULL, 'j'},
		{"clocks"          , no_argument       , NULL, 'k'},
		{"limits"          , no_argument       , NULL, 'l'},
		{"locale"          , no_argument       , NULL, 'L'},
		{"mounts"          , no_argument       , NULL, 'm'},
		{"message-queues"  , no_argument       , NULL, 'M'},
		{"confstr"         , no_argument       , NULL, 'n'},
		{"network"         , no_argument       , NULL, 'N'},
		{"oom"             , no_argument       , NULL, 'o'},
		{"process"         , no_argument       , NULL, 'p'},
		{"platform"        , no_argument       , NULL, 'P'},
		{"time"            , no_argument       , NULL, 'q'},
		{"ranges"          , no_argument       , NULL, 'r'},
		{"signals"         , no_argument       , NULL, 's'},
		{"shared-memory"   , no_argument       , NULL, 'S'},
		{"tty"             , no_argument       , NULL, 't'},
		{"threads"         , no_argument       , NULL, 'T'},
		{"stat"            , no_argument       , NULL, 'u'},
		{"rusage"          , no_argument       , NULL, 'U'},
		{"version"         , no_argument       , NULL, 'v'},
		{"capabilities"    , no_argument       , NULL, 'w'},
		{"pathconf"        , no_argument       , NULL, 'x'},
		{"sysconf"         , no_argument       , NULL, 'y'},
		{"memory"          , no_argument       , NULL, 'Y'},
		{"timezone"        , no_argument       , NULL, 'z'},
		{"exec"            , no_argument       , NULL,  0 },
		{"file"            , required_argument , NULL,  0 },
		{"file-append"     , no_argument       , NULL,  0 },
		{"format"          , required_argument , NULL,  0 },
		{"indent"          , required_argument , NULL,  0 },
		{"indent-char"     , required_argument , NULL,  0 },
		{"output"          , required_argument , NULL,  0 },
		{"separator"       , required_argument , NULL,  0 },

		/* terminator */
		{NULL              , no_argument       , NULL,  0 }
	};

	doc = pstring_new ();
	if (! doc)
		die ("failed to allocate string");

	program_name = argv[0];
	argvp = argv;
	argvc = argc;

	/* Check before command-line options, since the latter
	 * must take priority.
	 */
	check_envvars ();

	init ();

	while (true) {
		option = getopt_long (argc, argv,
				"aAbBcCdeEfFghijklLmMnNopPqrsStTuUvwxyYz",
				long_options, &long_index);
		if (option == -1)
			break;

		/* If the user has specified a display option, only
		 * display that particular group (but crucially don't
		 * count non-display options).
		 */
		if (option) {
			done = true;
			master_header (&doc);
		}

		selected_option = option;

		switch (option)
		{
		case 0:
			/* The exception is '--exec' */
			if (done && strcmp ("exec", long_options[long_index].name))
				die ("Must specify non-display options before display options");

			if (! strcmp ("output", long_options[long_index].name)) {
				set_output_value (optarg);
			} else if (! strcmp ("file", long_options[long_index].name)) {
				set_output_value_raw (OUTPUT_FILE);
				set_output_file (optarg);
			} else if (! strcmp ("file-append", long_options[long_index].name)) {
				set_output_file_append ();
			} else if (! strcmp ("exec", long_options[long_index].name)) {
				reexec = true;
			} else if (! strcmp ("format", long_options[long_index].name)) {
				set_output_format (optarg);
			} else if (! strcmp ("indent", long_options[long_index].name)) {
				set_indent_amount (atoi (optarg));
				if (get_indent_amount () <= 0)
					die ("cannot specify indent <= 0");
			} else if (! strcmp ("indent-char", long_options[long_index].name)) {
				char *c = NULL;

				/* Special character handling */
				if (! strcmp (optarg, "\\t")) {
					c = "\t";
				} else {
					c = optarg;
				}
				if (! c)
					die ("cannot use nul indent character");

				set_indent_char (c);

				/* call again */
				handle_indent_char ();

			} else if (! strcmp ("separator", long_options[long_index].name)) {
				if (! strcmp (optarg, "\\t")) {
					set_text_separator ("\t");
				} else {
					set_text_separator (optarg);
				}
			} else if (! strcmp ("crumb-separator", long_options[long_index].name)) {
				if (! strcmp (optarg, "\\t")) {
					set_crumb_separator ("\t");
				} else {
					set_crumb_separator (optarg);
				}
			}
			/* reset */
			selected_option = 0;
			reset_indent ();

			break;

		case 'a':
			show_meta ();
			break;

		case 'A':
			show_arguments ();
			break;

		case 'b':
			show_libs ();
			break;

		case 'B':
			show_libc ();
			break;

		case 'c':
			show_cgroups ();
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

		case 'F':
			show_namespaces ();
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
			show_confstrs();
			break;

		case 'N':
			show_network ();
			break;

		case 'o':
			show_oom ();
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
			show_capabilities ();
			break;

		case 'x':
			show_mounts (SHOW_PATHCONF);
			break;

		case 'y':
			show_sysconf ();
			break;

		case 'Y':
			show_memory ();
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

		pstring_chomp (doc);

		if (output_format != OUTPUT_FORMAT_XML && output_format != OUTPUT_FORMAT_JSON) {
			pstring_compress (&doc, wide_indent_char);
		}
	}

	output_init ();

	if (reexec && ! exec_args && optind >= argc)
		die ("must specify atleast one argument with '--exec'");

	/* Prepare for re-exec */
	if (reexec) {
		if (! exec_args) {
			argv += optind;
			exec_args = argv;
		}
	}

	if (! done) {
		dump ();

		pstring_chomp (doc);

		pstring_compress (&doc, wide_indent_char);
	}

	_show_output_pstring (doc);
	cleanup ();

	/* Perform re-exec */
	if (reexec) {
		execvp (exec_args[0], exec_args);
		die ("failed to re-exec %s", exec_args[0]);
	}

	exit (EXIT_SUCCESS);

}

/* FIXME: we _assume_ the returned value is a static, but is it guaranteed?
*/
const char *
get_user_name (uid_t uid)
{
	struct passwd *p;

	p = getpwuid (uid);

	return p ? p->pw_name : NULL;
}

/* FIXME: we _assume_ the returned value is a static, but is it guaranteed?
*/
const char *
get_group_name (gid_t gid)
{
	struct group *g;

	g = getgrgid (gid);

	return g ? g->gr_name : NULL;
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
	header ("shared memory");

	if (ops->show_shared_mem)
		ops->show_shared_mem ();

	footer ();
}

void
show_semaphores (void)
{
	header ("semaphores");

	if (ops->show_semaphores)
		ops->show_semaphores ();

	footer ();
}

void
show_msg_queues (void)
{
	header ("message queues");

	if (ops->show_msg_queues)
		ops->show_msg_queues ();

	footer ();
}

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
	strncpy (buffer, str, len);

	/* Overwrite NL */
	buffer[strlen (str)-1] = '\0';
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
	modestr = calloc ((1+3+3+3)+1, sizeof (char));

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
#ifdef PROCENV_PLATFORM_ANDROID

// FIXME: split out to platfom
void
show_network (void)
{
	/* Bionic isn't actually that bionic at all :( */
	header ("network");
	footer ();
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
#if !defined (PROCENV_PLATFORM_HURD)
		int family;
#endif

		if (! ifa->ifa_addr)
			continue;

#if !defined (PROCENV_PLATFORM_HURD)
		family = ifa->ifa_addr->sa_family;

		if (family == PROCENV_LINK_LEVEL_FAMILY) {

			/* MAC address cannot be queries on minix seemingly */
#if !defined (PROCENV_PLATFORM_MINIX)
			/* Add link level interface details to the cache */
			mac_address = get_mac_address (ifa);
#endif

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

void
show_misc (void)
{
#if defined (PROCENV_PLATFORM_LINUX)
	int            domain = 0x0;

	/* magic value - see personality(2) */
	unsigned int   persona = 0xFFFFFFFF;

	unsigned int   mask = PER_MASK;

	const char    *personality_name = NULL;
	char          *personality_flags = NULL;
#endif

	header ("misc");

	entry ("umask", "%4.4o", misc.umask_value);
	entry ("current directory (cwd)", "'%s'", misc.cwd);
#if defined (PROCENV_PLATFORM_LINUX)
	entry ("root", "%s%s%s",
			strcmp (misc.root, UNKNOWN_STR) ? "'" : "",
			misc.root,
			strcmp (misc.root, UNKNOWN_STR) ? "'" : "");
#endif
	entry ("chroot", "%s", in_chroot () ? YES_STR : NO_STR);
	entry ("container", "%s", container_type ());

	entry ("vm", "%s",
            ops->in_vm ?
            ops->in_vm () ? YES_STR : NO_STR
            : UNKNOWN_STR);

	if (ops->show_prctl) {
		section_open ("prctl");
		ops->show_prctl ();
		section_close ();
	}

	if (ops->show_security_module) {
		section_open ("security module");
		ops->show_security_module ();
		section_close ();
	}

#if defined (PROCENV_PLATFORM_LINUX)
#ifdef LINUX_VERSION_CODE
	entry ("kernel headers version", "%u.%u.%u",
			(LINUX_VERSION_CODE >> 16),
			((LINUX_VERSION_CODE >> 8) & 0xFF),
			(LINUX_VERSION_CODE & 0xFF));
#endif

	domain = personality (persona);

	personality_name = get_personality_name (domain & mask);

	section_open ("personality");

	entry ("type", "%s", personality_name
			? personality_name
			: UNKNOWN_STR);

	personality_flags = get_personality_flags (domain & (-1 ^ mask));
	entry ("flags", "%s",
			personality_flags
			? personality_flags
			: "");

	section_close ();

	if (personality_flags)
		free (personality_flags);
#endif

	footer ();
}
