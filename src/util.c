/*--------------------------------------------------------------------
 * Copyright © 2016 James Hunt <jamesodhunt@gmail.com>.
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

#include "util.h"
#if __GLIBC__
#include <sys/sysmacros.h>
#endif

// FIXME
extern struct procenv_user user;
extern struct procenv_misc misc;

struct baud_speed
{
	char    *name;
	speed_t  speed;
};

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

#if !defined (PROCENV_PLATFORM_HURD) && \
    !defined (PROCENV_PLATFORM_MINIX) && \
    !defined (PROCENV_PLATFORM_DARWIN)

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
 * Returns: true if @fd is attached to a console, else false.
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

/**
 * is_big_endian:
 *
 * Returns: true if system is big-endian, else false.
 **/
bool
is_big_endian (void)
{
	int x = 1;

	if (*(char *)&x == 1)
		return false;

	return true;
}

bool
has_ctty (void)
{
	int fd;
	fd = open ("/dev/tty", O_RDONLY | O_NOCTTY);

	if (fd < 0)
		return false;

	close (fd);

	return true;
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
#if defined (PROCENV_PLATFORM_LINUX)
	dev_t        expected;

	expected = makedev (5, 1);
#endif

	if (stat ("/dev/console", &statbuf) < 0)
		goto out;

#if defined (PROCENV_PLATFORM_FREEBSD)
	if (misc.in_jail)
		return "jail";
#endif

	/* LXC's /dev/console is actually a pty */
#if defined (PROCENV_PLATFORM_LINUX)
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
 * Returns true if within a chroot, else false.
 **/
// FIXME: add different implementations?
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
		return true;

	i = stat (bsd_self, &st);
	if (i == 0) {
		/* Give up here if running on BSD */
		return false;
	}

	i = stat (self, &st);
	if (i != 0)
		return false;

	self_inode = st.st_ino;

	if (root_inode == self_inode)
		return false;

	return true;

error:
	die ("cannot stat '%s'", dir);

	/* compiler appeasement */
	return false;
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

static struct baud_speed baud_speeds[] = {
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
    { NULL, 0 }
};

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

