/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *--------------------------------------------------------------------
 */

#ifndef _PROCENV_TYPES_H
#define _PROCENV_TYPES_H

#include <stdint.h>

struct procenv_map {
	const char    *name;
	unsigned int   num;
};

struct procenv_map64 {
	const char    *name;
	uint64_t       num;
};

#endif /* _PROCENV_TYPES_H */
