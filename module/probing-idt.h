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

#ifndef _PROBING_IDT_H
#define _PROBING_IDT_H

#ifdef CONFIG_KALLSYMS
struct idt_entry {
	char name[128];
	unsigned long address;
} __attribute__((aligned(64)));
#else
struct idt_entry {
	unsigned long address;
} __attribute__((aligned(8)));
#endif

int collect_IDT_info(void);
struct idt_entry *get_IDT_entry(int);

#endif