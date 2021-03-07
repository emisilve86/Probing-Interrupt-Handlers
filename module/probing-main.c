/*****************************************************************************
 * Copyright  2021  Emisilve86                                               *
 *                                                                           *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 *****************************************************************************/

#include <linux/module.h>

#include "utils.h"
#include "probing-idt.h"
#include "probing-device.h"

static __init int forced_probing_init(void)
{
#ifdef CONFIG_X86_64
	pr_info("[Probing NOP] : " _WHITE_ "Module Initialization" _RESET_ "\n");

	if (collect_IDT_info())
		return -1;

	if (probing_device_init())
		return -1;

	pr_info("[Probing NOP] : " _WHITE_ "Initialization Completed" _RESET_ "\n");

	return 0;
#else
	pr_info("[Probing NOP] : " _WHITE_ "This Kernel has not been compiled for x86-64 architectures" _RESET_ "\n");

	return -1;
#endif
}

static __exit void forced_probing_fini(void)
{
#ifdef CONFIG_X86_64
	pr_info("[Probing NOP] : " _WHITE_ "Module Finalization" _RESET_ "\n");

	probing_device_fini();

	pr_info("[Probing NOP] : " _WHITE_ "Finalization Completed" _RESET_ "\n");
#endif
}

module_init(forced_probing_init);
module_exit(forced_probing_fini);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Emiliano Silvestri <silvestri@diag.uniroma1.it>");
MODULE_DESCRIPTION("Force installation of CALLs to probing functions in place of multi-byte NOPs encounterd within interrupt routines");