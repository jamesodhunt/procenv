#ifndef PROCENV_H
#define PROCENV_H

/* for dl_iterate_phdr(3) */
#define _GNU_SOURCE

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <wchar.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <paths.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sched.h>
#include <termios.h>
#include <getopt.h>
#include <assert.h>
#include <sys/types.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <link.h>
#include <fenv.h>
#include <sys/utsname.h>
#include <locale.h>

#if defined (__FreeBSD__) \
	|| defined (__NetBSD__) \
	|| defined (__OpenBSD__)
#define PROCENV_BSD
#endif

#ifdef linux
#define PROCENV_LINUX
#endif

#ifdef __GNU__
#define PROCENV_HURD
#endif

#if defined (__i386__) || defined (__x86_64__)
#define PROCENV_ARCH_X86
#endif

#if defined (PROCENV_LINUX)
#include <mntent.h>
#include <execinfo.h>
#include <sys/inotify.h>
#include <sys/prctl.h>

#include <linux/prctl.h>
#include <linux/version.h>

/* Lucid provides prctl.h, but not securebits.h */
#if defined (PR_GET_SECUREBITS) && defined (HAVE_LINUX_SECUREBITS_H)
#include <linux/securebits.h>
#endif

#include <linux/capability.h>
#include <linux/vt.h>
#ifdef HAVE_APPARMOR
#include <sys/apparmor.h>
#endif
#ifdef HAVE_SELINUX
#include <selinux/selinux.h>
#endif

/**
 * show_capability:
 * @cap: capability.
 *
 * Display specified capability, or NOT_DEFINED_STR if value is
 * unknown.
 **/
#define show_capability(cap) \
{ \
	ret = prctl (PR_CAPBSET_READ, cap, 0, 0, 0); \
	\
	show (#cap "=%s", ret < 0 \
			? NOT_DEFINED_STR \
			: ret \
			? YES_STR \
			: NO_STR); \
}

/**
 * LINUX_KERNEL_M:
 * @major: Linux major kernel version number.
 *
 * Returns: TRUE if running Linux kernel is atleast at version
 * specified by @major else FALSE.
 **/
#define LINUX_KERNEL_M(major) \
    (linux_kernel_version (major, -1, -1))

/**
 * LINUX_KERNEL_MM:
 * @major: Linux major kernel version number,
 * @minor: Linux minor kernel version number.
 *
 * Returns: TRUE if running Linux kernel is atleast at version
 * specified by (@major, @minor) else FALSE.
 **/
#define LINUX_KERNEL_MM(major, minor) \
    (linux_kernel_version (major, minor, -1))

/**
 * LINUX_KERNEL_MMR:
 * @major: Linux major kernel version number,
 * @minor: Linux minor kernel version number,
 * @revision: kernel revision version.
 *
 * Returns: TRUE if running Linux kernel is atleast at version
 * specified by (@major, @minor, @revision) else FALSE.
 **/
#define LINUX_KERNEL_MMR(major, minor, revision) \
    (linux_kernel_version (major, minor, revision))

#endif

#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
#include <kvm.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <sys/ucred.h>
#endif

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
#include <sys/mount.h>
#endif

/* Horrid hack for Hurd... :-( */
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#define PROCENV_OUTPUT_ENV "PROCENV_OUTPUT"
#define PROCENV_FILE_ENV   "PROCENV_FILE"
#define PROCENV_EXEC_ENV   "PROCENV_EXEC"

#define PROCENV_BUFFER     1024
#define MOUNTS            "/proc/mounts"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE (!TRUE)
#endif

/* FIXME: gettext */
#define _(str) str

#define YES_STR          _("yes")
#define NO_STR           _("no")
#define NA_STR           _("n/a")
#define UNKNOWN_STR      _("unknown")
#define MAX_STR          _(" (max)")
#define DEFINED_STR      _("defined")
#define NOT_DEFINED_STR  _("not defined")
#define BIG_STR          _("big")
#define LITTLE_STR       _("little")

#if defined (PROCENV_BSD)
		/* SIGTHL is hidden by default */
#define NUM_SIGNALS 31
#else
#define NUM_SIGNALS 32
#endif

/* If an indent is required, use this many spaces */
#define INDENT 2

#define PROGRAM_AUTHORS "James Hunt <james.hunt@ubuntu.com>"

#define show(...) _show ("", indent, __VA_ARGS__)
#define showi(_indent, ...) _show ("", indent+_indent, __VA_ARGS__)

#define warn(...) \
{ \
	_show ("WARNING", 0, __VA_ARGS__); \
}

#define die(...) \
{ \
	_show ("ERROR", 0, __VA_ARGS__); \
	exit (EXIT_FAILURE); \
}

#define type_hex_width(type) \
	(sizeof (type) * 2)

#define get_group_name(gid) \
({struct group *g = getgrgid (gid); \
 g ? g->gr_name : NULL;})

#define get_user_name(uid) \
({struct passwd *p = getpwuid (uid); \
 p ? p->pw_name : NULL;})

#define show_clock_res(clock) \
{ \
	struct timespec res; \
	if (clock_getres (clock, &res) < 0) \
		die ("failed to query resolution for clock " #clock); \
	show ("%s: resolution: %ld.%09lds", #clock, res.tv_sec, res.tv_nsec); \
}

#define show_const(t, flag, constant) \
    show ("%s:%s=%d", #flag, #constant, !!(t.flag & constant))

#define show_cc(t, elem) \
    show ("  c_cc[%s]:0x%x", #elem, t.c_cc[elem]);

#define show_pathconf(what, path, name) \
{ \
	long conf; \
	errno = 0; \
	conf = pathconf (path, name); \
	if (conf == -1 && errno == 0) { \
		die ("unable to query pathconf value for '%s'", #name); \
	} \
	showi ((what == SHOW_ALL ? (indent * 2) : INDENT), "%s=%d", #name, conf); \
}

#define SPEED(s) \
	{s, #s }

#define mk_map_entry(s) \
	{s, #s }

#define show_confstr(s) \
{ \
	size_t len; \
	char *buffer; \
	\
	errno = 0; \
	len = confstr(s, NULL, 0); \
	assert (len && errno == 0); \
	\
	buffer = calloc (1, len); \
	if (! buffer) { \
		die ("failed to allocate space for confstr"); \
	} \
	assert (confstr (s, buffer, len) == len); \
	show ("%s: '%s'", #s, buffer); \
	free (buffer); \
}

#define is_limit_max(l) \
	((unsigned long int)l == ((unsigned long int)-1) ? MAX_STR : "")

#define show_limit(limit) \
{ \
	struct rlimit tmp; \
	if (getrlimit (limit, &tmp) < 0) { \
		die ("failed to query rlimit '%s'", #limit); \
	} \
	show (#limit " (soft=%lu%s, hard=%lu%s)", \
			(unsigned long int)tmp.rlim_cur, \
			is_limit_max (tmp.rlim_cur), \
			(unsigned long int)tmp.rlim_max, \
			is_limit_max (tmp.rlim_max)); \
}

#define show_usage(rusage, name) \
	show ("%s=%lu", #name, rusage.name)

#define get_sysconf(s) \
 	sysconf (s)

#define show_sysconf(s) \
{ \
	long value = get_sysconf (s); \
	show (#s "=%ld", value); \
}

#define mk_posix_sysconf_map_entry(name) \
	{_SC_ ## name, #name "(_SC_" #name ")" }

#define mk_posixopt_sysconf_map_entry(name) \
	{_SC_ ## name, "_POSIX_" #name "(_SC_" #name ")" }

#define mk_sysconf_map_entry(name) \
	{name, #name }

typedef char bool;

typedef enum {
	SHOW_ALL,
	SHOW_MOUNTS,
	SHOW_PATHCONF
} ShowMountType;

struct procenv_map {
	int   num;
	char *name;
};

typedef enum procenv_output {
	OUTPUT_FILE,
	OUTPUT_STDERR,
	OUTPUT_STDOUT,
	OUTPUT_SYSLOG,
	OUTPUT_TERM
} Output;

struct baud_speed
{
    speed_t  speed;
    char    *name;
};

struct procenv_user {
	pid_t pid;
	pid_t ppid;
	pid_t sid;

#if defined (PROCENV_LINUX)
	char proc_name[16];
#endif
#if defined (PROCENV_BSD) \
	|| defined (__FreeBSD_kernel__) \
	|| defined (PROCENV_HURD)
	char proc_name[COMMLEN+1];
#endif

	pid_t pgroup;
	pid_t fg_pgroup;
	pid_t pgid_sid;
	char  ctrl_terminal[L_ctermid];
	int   tty_fd;

	uid_t uid;
	uid_t euid;
	uid_t suid;

	char *login;

	gid_t gid;
	gid_t egid;
	gid_t sgid;

	struct passwd passwd;
};

struct procenv_misc {
	char   cwd[PATH_MAX];
	char   root[PATH_MAX];
	mode_t umask_value;
	int cpu;
#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
	int    in_jail;
#endif
};

struct procenv_priority {
	int process;
	int pgrp;
	int user;
};

void _show (const char *prefix, int indent, const char *fmt, ...);

void init (void);
void cleanup (void);
bool in_chroot (void);
const char *container_type (void);
bool is_process_group_leader (void);
bool is_session_leader (void);
void assert_cwd (void);
void assert_chroot (void);
void assert_user (void);
void dump_options (void);
void dump_user (void);
void dump_misc (void);
void show_env (void);
void show_rlimits (void);
void show_rusage (void);
void dump_sysconf (void);
void show_confstrs (void);
void dump_priorities (void);
void show_mounts (ShowMountType what);
void get_user_info (void);
void get_priorities (void);
void get_config (void);
void get_config_from_env (void);
void check_config (void);
void show_proc_branch (void);
void show_tty_attrs (void);
const char * get_speed (speed_t speed);
const char * get_signal_name (int signum);
void dump_meta (void);
char *get_platform (void);
int libs_callback (struct dl_phdr_info *info, size_t size, void *data);
void show_libs (void);
int get_indent (void);
void show_clocks (void);
void show_timezone (void);
void show_time (void);
void show_signals (void);
void show_pathconfs (ShowMountType what, const char *dir);
void show_sizeof (void);
void show_ranges (void);
void show_compiler (void);
void get_uname (void);
void dump_uname (void);
void show_all_groups (void);
int is_console (int fd);
long get_kernel_bits (void);
bool has_ctty (void);
void show_cpu (void);
void append (char **str, const char *new);
void appendf (char **str, const char *fmt, ...);
void appendva (char **str, const char *fmt, va_list ap);
void check_envvars (void);
int get_output_value (const char *name);
void set_indent (void);
void show_stat (void);
void get_major_minor (const char *path, int *major, int *minor);
bool uid_match (uid_t uid);
char * get_path (const char *argv0);
bool is_big_endian (void);

#if defined (PROCENV_LINUX)
void get_root (char *root, size_t len);
void dump_linux_proc_fds (void);
void show_linux_cgroups (void);
void show_oom (void);
void show_capabilities (void);
void show_linux_security_module (void);
void show_linux_security_module_context (void);
void show_linux_mounts (ShowMountType what);
void show_linux_proc_branch (void);
void show_linux_prctl (void);
void show_linux_cpu (void);
char * get_scheduler_name (int sched);
void show_linux_scheduler (void);
bool linux_kernel_version (int major, int minor, int revision);
#endif /* PROCENV_LINUX */

#if defined (PROCENV_BSD) || defined (__FreeBSD_kernel__)
char * get_bsd_mount_opts (uint64_t flags);
void show_bsd_mounts (ShowMountType what);
void get_bsd_misc (void);
void show_bsd_proc_branch (void);
void show_bsd_cpu (void);
#endif /* PROCENV_BSD + __FreeBSD_kernel__ */

#endif /* PROCENV_H */
