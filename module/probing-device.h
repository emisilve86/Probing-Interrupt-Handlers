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

#ifndef _PROBING_DEVICE_H
#define _PROBING_DEVICE_H

#include <linux/types.h>

#define P_IDT_SELECT	4
#define P_NOP_SELECT	8
#define P_NOP_UPDATE	16
#define P_NOP_RESTORE	32

struct probing_function {
	unsigned int nop_updated;
	unsigned int nop_rep;
	unsigned int nop_size;
	unsigned long nop_address;
	struct probing_function *next;
	struct probing_function *prev;
} __attribute__((packed, aligned(8)));

struct probing_function_list {
	struct probing_function *head;
	struct probing_function *tail;
	unsigned int count;
} __attribute__((aligned(8)));

struct probing_device {
	struct probing_function_list list;
	atomic_t used;
} __attribute__((aligned(8)));

int probing_device_init(void);
void probing_device_fini(void);
int probing_device_append(unsigned int, unsigned int, unsigned long);

int p_open(struct inode *, struct file *);
int p_release(struct inode *, struct file *);
ssize_t p_read(struct file *, char __user *, size_t, loff_t *);
long p_ioctl(struct file *, unsigned int, unsigned long);

#endif