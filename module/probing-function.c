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

#include "probing-function.h"

/*****************************************************************************
 * Here is where you have to declare all the functions (including variables) *
 * that are desired to be shown in the list of available probing functions,  *
 * whose addresses are valid values in order to update the selected NOP.     *
 *****************************************************************************/

#include <linux/version.h>
#include <asm/atomic.h>

atomic_t example_var = { .counter = 0 };

DECLARE_PF(example_foo_add)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
	arch_atomic_inc(&example_var);
#else
	atomic_inc(&example_var);
#endif
	return;
}

DECLARE_PF(example_foo_sub)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
	arch_atomic_dec(&example_var);
#else
	atomic_dec(&example_var);
#endif
	return;
}

/*****************************************************************************
 * Once all the available probing funtions have been declared, rembere to    *
 * insert the relative wrapper within the array devoted to keep track of any *
 * one of them.                                                              *
 *****************************************************************************/

static pf_wrapper pf_array[] = {
	WRAP_PF(example_foo_add),
	WRAP_PF(example_foo_sub),
	WRAP_END /* DO NOT REMOVE */
};

int get_pf_array_size(void)
{
	int i;

	for (i=0; pf_array[i].pf_ptr != NULL; i++);

	return i;
}

char *get_pf_name(int i)
{
	if (i < 0 || i >= get_pf_array_size())
		return NULL;

	return pf_array[i].pf_name;
}

unsigned long get_pf_address(int i)
{
	if (i < 0 || i >= get_pf_array_size())
		return 0;

	return (unsigned long) pf_array[i].pf_ptr;
}

int check_pf_exists(unsigned long addr)
{
	int i;

	for (i=0; i<get_pf_array_size(); i++)
		if (addr == get_pf_address(i))
			return 1;

	return 0;
}