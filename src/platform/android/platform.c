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

#include "platform.h"

struct procenv_ops platform_ops =
{
	.driver                        = PROCENV_SET_DRIVER (android),

	// FIXME
#if 0
	.init                          = NULL,
	.signal_map                    = &signal_map_linux,
	.get_time                      = get_time_generic,

	.show_clocks                   = show_clocks_generic,
	.show_fds                      = show_fds_linux,
	.show_namespaces               = show_namespaces_linux,
	.show_cgroups                  = show_cgroups_linux,
	.show_oom                      = show_oom_linux,
	.show_timezone                 = show_timezone_linux,
	.show_capabilities             = show_capabilities_linux,
	.show_security_module          = show_security_module_linux,
	.show_security_module_context  = show_security_module_context_linux,
	.show_prctl                    = show_prctl_linux,
	.show_cpu                      = show_cpu_linux,
	.show_proc_branch              = show_proc_branch_linux,
	.show_shared_mem               = show_shared_mem_linux,
	.show_semaphores               = show_semaphores_linux,
	.show_msg_queues               = show_msg_queues_linux,
	.show_io_priorities            = show_io_priorities_linux,
#endif
};
