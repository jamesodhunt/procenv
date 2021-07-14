/*--------------------------------------------------------------------
 * Copyright (c) 2016-2021 James O. D. Hunt <jamesodhunt@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
