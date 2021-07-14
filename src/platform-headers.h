/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

/*--------------------------------------------------------------------
 * Description: This file contains platform-specific system includes
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_PLATFORM_HEADERS_H
#define _PROCENV_PLATFORM_HEADERS_H

/*------------------------------------------------------------------*/

#if defined (PROCENV_PLATFORM_DARWIN)

#include <sys/mount.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <sys/statvfs.h>
#include <sys/ucred.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <mach/clock.h>
#include <mach/mach.h>
#include <sys/sysctl.h>

#define	PROCENV_LINK_LEVEL_FAMILY  AF_LINK
#define PROCENV_CPU_TYPE           int

/* XXX: fake value as cpu set are not available on darwin */
#define PROCENV_CPU_SET_TYPE       void

#define PROCENV_MNT_GET_FLAGS(mnt) (mnt)->f_flags
#define PROCENV_MNT_GET_FSID(mnt)  (mnt)->f_fsid.val

#define PROCENV_STATFS_INT_TYPE    uint64_t
#define PROCENV_STATFS_INT_FMT     PRIu64

typedef struct statfs procenv_mnt_type;

#endif /* PROCENV_PLATFORM_DARWIN*/

/*------------------------------------------------------------------*/

/* XXX: for now, let's assume "unknown" is similar to Linux :-) */
#if defined (PROCENV_PLATFORM_LINUX) || defined (PROCENV_PLATFORM_GENERIC)

#include <sys/statvfs.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <sched.h>
#include <link.h>

#include <linux/if.h>

#include <sys/inotify.h>
#include <sys/prctl.h>
#include <sys/personality.h>
#include <sys/syscall.h>

#ifndef _SYS_PRCTL_H
#include <linux/prctl.h>
#endif

#include <linux/version.h>

/* Lucid provides prctl.h, but not securebits.h */
#if defined (PR_GET_SECUREBITS) && defined (HAVE_LINUX_SECUREBITS_H)
#include <linux/securebits.h>
#endif

#include <linux/capability.h>
#include <linux/vt.h>

#if defined (HAVE_SYS_APPARMOR_H)
#include <sys/apparmor.h>
#endif

#if defined (HAVE_SELINUX_SELINUX_H)
#include <selinux/selinux.h>
#endif

#if defined (HAVE_SYS_CAPABILITY_H)
#include <sys/capability.h>
#endif

#if defined (__GLIBC__)
#include <sys/sysmacros.h>
#endif

/* Network family for entries containing link-level interface
 * details. These entries will be cached to allow MAC addresses
 * to be extracted from them when displaying the corresponding
 * higher-level network family entries for the interface in
 * question.
 */
#define PROCENV_LINK_LEVEL_FAMILY  AF_PACKET
#define PROCENV_PTHREAD_GUARD_SIZE_TYPE  size_t
#define PROCENV_PTHREAD_GUARD_SIZE_FMT   "%lu"

#define PROCENV_CPU_SET_TYPE       cpu_set_t
#define PROCENV_CPU_TYPE           int

#endif /* PROCENV_PLATFORM_LINUX || PROCENV_PLATFORM_GENERIC */

/*------------------------------------------------------------------*/

#if defined (PROCENV_PLATFORM_MINIX)

#include <sched.h>
#include <sys/mount.h>
#include <sys/fstypes.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/statvfs.h>
#include <sys/ucred.h>
#include <net/if.h>
#include <netinet/in.h>
#include <kvm.h>
#include <link.h>

#define	PROCENV_LINK_LEVEL_FAMILY        AF_LINK
#define PROCENV_PTHREAD_GUARD_SIZE_TYPE  int
#define PROCENV_PTHREAD_GUARD_SIZE_FMT   "%u"
#define PROCENV_CPU_TYPE                 cpuid_t
#define PROCENV_CPU_SET_TYPE             cpuset_t
#define PROCENV_MNT_GET_FLAGS(mnt)       (mnt)->f_flag
#define PROCENV_MNT_GET_FSID(mnt)        (mnt)->f_fsidx.__fsid_val

#define PROCENV_STATFS_INT_TYPE    uint64_t
#define PROCENV_STATFS_INT_FMT     PRIu64

typedef struct statvfs procenv_mnt_type;

#endif /* PROCENV_PLATFORM_MINIX */

/*------------------------------------------------------------------*/

#if defined (PROCENV_PLATFORM_FREEBSD)

#include <sys/mount.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <sys/statvfs.h>
#include <sys/ucred.h>
#include <sys/consio.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <kvm.h>
#include <pthread_np.h>
#include <link.h>

#define	PROCENV_LINK_LEVEL_FAMILY  AF_LINK
#define PROCENV_CPU_TYPE           int
#define PROCENV_CPU_SET_TYPE       cpuset_t
#define PROCENV_MNT_GET_FLAGS(mnt) (mnt)->f_flags
#define PROCENV_MNT_GET_FSID(mnt)  (mnt)->f_fsid.val

#define PROCENV_STATFS_INT_TYPE    uint64_t
#define PROCENV_STATFS_INT_FMT     PRIu64

typedef struct statfs procenv_mnt_type;

#endif /* PROCENV_PLATFORM_FREEBSD*/

/*------------------------------------------------------------------*/

#if defined (PROCENV_PLATFORM_NETBSD)

/* Required to access "struct kinfo_proc" (from sys/sysctl.h) */
#ifndef _KMEMUSER
#define _KMEMUSER
#endif

#include <sched.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/statvfs.h>
#include <sys/ucred.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <kvm.h>
#include <dev/wscons/wsdisplay_usl_io.h>
#include <link.h>

#define	PROCENV_LINK_LEVEL_FAMILY  AF_LINK
#define PROCENV_CPU_TYPE           cpuid_t
#define PROCENV_CPU_SET_TYPE       cpuset_t
#define PROCENV_MNT_GET_FLAGS(mnt) (mnt)->f_flag
#define PROCENV_MNT_GET_FSID(mnt)  (mnt)->f_fsidx.__fsid_val

#define PROCENV_STATFS_INT_TYPE    uint64_t
#define PROCENV_STATFS_INT_FMT     PRIu64

typedef struct statvfs procenv_mnt_type;

#endif /* PROCENV_PLATFORM_NETBSD */

/*------------------------------------------------------------------*/

#if defined (PROCENV_PLATFORM_OPENBSD)

#include <sched.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/proc.h>

#include <sys/statvfs.h>
#include <sys/ucred.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <kvm.h>
#include <dev/wscons/wsdisplay_usl_io.h>
#include <link.h>

#define	PROCENV_LINK_LEVEL_FAMILY  AF_LINK
#define PROCENV_CPU_TYPE           cpuid_t
#define PROCENV_CPU_SET_TYPE       struct cpuset
#define PROCENV_MNT_GET_FLAGS(mnt) (mnt)->f_flags
#define PROCENV_MNT_GET_FSID(mnt)  (mnt)->f_fsid.val

#define PROCENV_STATFS_INT_TYPE    uint64_t
#define PROCENV_STATFS_INT_FMT     PRIu64

typedef struct statfs procenv_mnt_type;

#endif /* PROCENV_PLATFORM_OPENBSD */

/*------------------------------------------------------------------*/

#if defined (PROCENV_PLATFORM_HURD)

#include <mntent.h>
#include <sys/vfs.h>
#include <sys/statvfs.h>
#include <net/if.h>
#include <link.h>
#include <sys/sysmacros.h>

#if defined (HAVE_SYS_CAPABILITY_H)
#include <sys/capability.h>
#endif

#define PROCENV_CPU_TYPE     int
#define PROCENV_CPU_SET_TYPE cpu_set_t

#endif /* PROCENV_PLATFORM_HURD */

/*------------------------------------------------------------------*/

#if defined (PROCENV_PLATFORM_GENERIC)

/* XXX: see above */

#endif /* PROCENV_PLATFORM_GENERIC */

/*------------------------------------------------------------------*/

#endif /* _PROCENV_PLATFORM_HEADERS_H */

/*------------------------------------------------------------------*/
