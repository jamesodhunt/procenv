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

/*--------------------------------------------------------------------
 * Description: This file contains platform-specific system includes
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_PLATFORM_HEADERS_H
#define _PROCENV_PLATFORM_HEADERS_H

/*------------------------------------------------------------------*/

/* XXX: for now, let's assume "unknown" is similar to Linux :-) */
#if defined (PROCENV_PLATFORM_LINUX) || defined (PROCENV_PLATFORM_GENERIC)

#include <sys/statvfs.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <sched.h>

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

#define PROCENV_CPU_SET_TYPE cpu_set_t
#define PROCENV_CPU_TYPE     int

#endif /* PROCENV_PLATFORM_LINUX */

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

#define PROCENV_CPU_TYPE           cpuid_t
#define PROCENV_CPU_SET_TYPE       cpuset_t
#define PROCENV_MNT_GET_FLAGS(mnt) (mnt)->f_flag
#define PROCENV_MNT_GET_FSID(mnt)  (mnt)->f_fsidx.__fsid_val

#define PROCENV_STATFS_INT_TYPE    uint64_t
#define PROCENV_STATFS_INT_FMT     PRIu64

typedef struct statvfs procenv_mnt_type;

#endif /* PROCENV_PLATFORM_NETBSD */

/*------------------------------------------------------------------*/

#if defined (PROCENV_PLATFORM_HURD)

#include <mntent.h>
#include <sys/vfs.h>
#include <sys/statvfs.h>
#include <net/if.h>

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
