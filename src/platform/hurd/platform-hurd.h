/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_PLATFORM_HURD_H
#define _PROCENV_PLATFORM_HURD_H

#include "platform.h"
#include "util.h"

/* Seriously? */
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* semctl(2) states that POSIX.1-2001 requires the caller define this! */
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};

#endif /* _PROCENV_PLATFORM_HURD_H */
