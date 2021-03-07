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

#ifdef CONFIG_KALLSYMS
#include <linux/kallsyms.h>
#endif
#include <asm/desc.h>
#include <asm/desc_defs.h>

#include "utils.h"
#include "probing-idt.h"

static struct idt_entry i_entry[NR_VECTORS];

int collect_IDT_info(void)
{
	int i, err;

	gate_desc *gate_ptr;
	struct desc_ptr idtr;

	err = 0;

	store_idt(&idtr);

	if (idtr.address)
	{
		for (i=0; i<NR_VECTORS; i++)
		{
			gate_ptr = (gate_desc *) (idtr.address + (i * sizeof(gate_desc)));

			i_entry[i].address = (((unsigned long) gate_ptr->offset_low) |
									((unsigned long) gate_ptr->offset_middle << 16) |
										((unsigned long) gate_ptr->offset_high << 32));
#ifdef CONFIG_KALLSYMS
			if (sprint_symbol(i_entry[i].name, i_entry[i].address) == 0)
				i_entry[i].name[0] = '\0';
#endif
		}
	}
	else
	{
		pr_err("[Probing NOP] : " _WHITE_ "Unable to access IDT information" _RESET_ "\n");
		err = -1;
	}

	return err;
}

struct idt_entry *get_IDT_entry(int i)
{
	if (i >= 0 && i < NR_VECTORS)
		return &i_entry[i];

	return NULL;
}