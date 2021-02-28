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

#include "platform-unknown.h"

static struct procenv_map signal_map_generic[] = {

	mk_map_entry (SIGABRT),
	mk_map_entry (SIGALRM),
	mk_map_entry (SIGBUS),

	{ SIGCHLD, "SIGCHLD|SIGCLD" },

	mk_map_entry (SIGCONT),

#if defined SIGEMT
	mk_map_entry (SIGEMT),
#endif

	mk_map_entry (SIGFPE),
	mk_map_entry (SIGHUP),
	mk_map_entry (SIGILL),

#if defined SIGINFO
	mk_map_entry (SIGINFO),
#endif

	mk_map_entry (SIGINT),

#if defined SIGIO
	mk_map_entry (SIGIO),
#endif

#if defined SIGIOT
	mk_map_entry (SIGIOT),
#endif

	mk_map_entry (SIGKILL),
	mk_map_entry (SIGPIPE),
	mk_map_entry (SIGPOLL),
	mk_map_entry (SIGPROF),

#if defined SIGPWR
	mk_map_entry (SIGPWR),
#endif

	mk_map_entry (SIGQUIT),
	mk_map_entry (SIGSEGV),
	mk_map_entry (SIGSTOP),
	mk_map_entry (SIGSYS),
	mk_map_entry (SIGTERM),
	mk_map_entry (SIGTRAP),
	mk_map_entry (SIGTSTP),
	mk_map_entry (SIGTTIN),
	mk_map_entry (SIGTTOU),
	mk_map_entry (SIGURG),
	mk_map_entry (SIGUSR1),
	mk_map_entry (SIGUSR2),
	mk_map_entry (SIGVTALRM),
	mk_map_entry (SIGWINCH),
	mk_map_entry (SIGXCPU),
	mk_map_entry (SIGXFSZ),

	{ 0, NULL },
};

struct procenv_ops platform_ops =
{
	.driver                        = PROCENV_SET_DRIVER (unknown),

	.get_time                      = get_time_generic,

	.signal_map                    = signal_map_generic,
	.show_fds                      = show_fds_generic,
	.show_clocks                   = show_clocks_generic,
};
