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

#ifndef _PROBING_SESSION_H
#define _PROBING_SESSION_H

#include "probing-idt.h"
#include "probing-device.h"

struct probing_session {
	int idt_select;
	unsigned long nop_select;
	struct idt_entry *idt_entry;
} __attribute__((packed, aligned(8)));

ssize_t binary_analysis(struct probing_session *, char __user *, size_t);
int insert_probe_function(struct probing_function *, unsigned long);
void remove_probe_function(struct probing_function *);

#endif