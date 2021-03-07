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

#ifndef _PROBING_FUNCTION_H
#define _PROBING_FUNCTION_H

#include <stddef.h>

#define WRAP_END		{ .pf_name = NULL, .pf_ptr = NULL }
#define WRAP_PF(NAME)	{ .pf_name = #NAME, .pf_ptr = entry_pf_##NAME }

#define DECLARE_PF(NAME) 									\
void pf_##NAME(void);										\
void entry_pf_##NAME(void);									\
asm(														\
	"	.type	entry_pf_" #NAME ", @function\n"			\
	"	.align	4, 0x90\n"									\
	"entry_pf_" #NAME ":\n"									\
	"	pushq	%rax\n"										\
	"	pushq	%rcx\n"										\
	"	pushq	%rdx\n"										\
	"	pushq	%rsi\n"										\
	"	pushq	%rdi\n"										\
	"	pushq	%r8\n"										\
	"	pushq	%r9\n"										\
	"	pushq	%r10\n"										\
	"	pushq	%r11\n"										\
	"	callq	pf_" #NAME "\n"								\
	"	popq	%r11\n"										\
	"	popq	%r10\n"										\
	"	popq	%r9\n"										\
	"	popq	%r8\n"										\
	"	popq	%rdi\n"										\
	"	popq	%rsi\n"										\
	"	popq	%rdx\n"										\
	"	popq	%rcx\n"										\
	"	popq	%rax\n"										\
	"	ret\n"												\
	"	.size	entry_pf_" #NAME ", .-entry_pf_" #NAME "\n"	\
);															\
void pf_##NAME(void)

typedef struct __pf_wrapper {
	char *pf_name;
	void (*pf_ptr)(void);
} pf_wrapper;

int get_pf_array_size(void);
char *get_pf_name(int);
unsigned long get_pf_address(int);
int check_pf_exists(unsigned long);

#endif