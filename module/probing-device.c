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

#include <linux/version.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

#include "utils.h"
#include "probing-idt.h"
#include "probing-device.h"
#include "probing-session.h"
#include "probing-function.h"

static struct file_operations p_fops =
{
	.open           = p_open,
	.owner          = THIS_MODULE,
	.release        = p_release,
	.read			= p_read,
	.unlocked_ioctl = p_ioctl,
};

static int p_major;
static struct class *p_class;
static struct probing_device p_dev;

static char *probing_devnode(struct device *dev, umode_t *mode)
{
	return kasprintf(GFP_KERNEL, "probing-nop");
}

static int probing_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0666);
	return 0;
}

int probing_device_init(void)
{
	int err = 0;
	struct device *dev;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
	arch_atomic_set(&p_dev.used, 0);
#else
	atomic_set(&p_dev.used, 0);
#endif

	p_dev.list.head = NULL;
	p_dev.list.tail = NULL;
	p_dev.list.count = 0;

	p_major = __register_chrdev(0, 0, 1, "probing-nop", &p_fops);
	if (p_major < 0)
	{
		pr_err("[Probing NOP] : " _WHITE_ "Unable to register the character device" _RESET_ "\n");
		err = -1;
		goto out;
	}

	p_class = class_create(THIS_MODULE, "probing-nop");
	if (IS_ERR(p_class))
	{
		pr_err("[Probing NOP] : " _WHITE_ "Unable to create the device class" _RESET_ "\n");
		err = -1;
		goto unreg;
	}

	p_class->devnode = probing_devnode;
	p_class->dev_uevent = probing_uevent;

	dev = device_create(p_class, NULL, MKDEV(p_major, 0), NULL, "probing-nop");
	if (IS_ERR(dev))
	{
		pr_err("[Probing NOP] : " _WHITE_ "Unable to create the device" _RESET_ "\n");
		err = -1;
		goto dest;
	}

	goto out;

dest:
	class_destroy(p_class);
unreg:
	__unregister_chrdev(p_major, 0, 1, "probing-nop");
out:
	return err;
}

void probing_device_fini(void)
{
	struct probing_function *p_fun;

	device_destroy(p_class, MKDEV(p_major, 0));

	class_destroy(p_class);

	__unregister_chrdev(p_major, 0, 1, "probing-nop");

	while ((p_fun = p_dev.list.head) != NULL)
	{
		p_dev.list.head = p_fun->next;
		p_dev.list.count--;

		remove_probe_function(p_fun);

		kfree((void *) p_fun);
	}
}

int probing_device_append(unsigned int rep, unsigned int size, unsigned long addr)
{
	struct probing_function *p_fun = p_dev.list.head;

	if (addr == 0)
		return -1;

	if (size < 5 || size > 9)
		return -1;

	while (p_fun)
	{
		if (p_fun->nop_address == addr)
			return 0;

		p_fun = p_fun->next;
	}

	if ((p_fun = (struct probing_function *) kmalloc(sizeof(struct probing_function), GFP_KERNEL)) == NULL)
		return -1;

	p_fun->nop_updated = 0;
	p_fun->nop_rep = (rep) ? 1 : 0;
	p_fun->nop_size = size;
	p_fun->nop_address = addr;

	p_fun->next = NULL;

	if ((p_fun->prev = p_dev.list.tail) == NULL)
		p_dev.list.head = p_fun;
	else
		p_dev.list.tail->next = p_fun;

	p_dev.list.tail = p_fun;

	p_dev.list.count++;

	return 0;
}

int p_open(struct inode *node, struct file *filp)
{
	struct probing_session *p_ses;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
	if (arch_atomic_cmpxchg(&p_dev.used, 0, 1) != 0)
#else
	if (atomic_cmpxchg(&p_dev.used, 0, 1) != 0)
#endif
		return -1;

	if ((p_ses = (struct probing_session *) kmalloc(sizeof(struct probing_session), GFP_KERNEL)) == NULL)
	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
		arch_atomic_set(&p_dev.used, 0);
#else
		atomic_set(&p_dev.used, 0);
#endif
		return -1;
	}

	p_ses->idt_select = -1;
	p_ses->nop_select = 0;
	p_ses->idt_entry = NULL;

	filp->private_data = (void *) p_ses;

	return 0;
}

int p_release(struct inode *node, struct file *filp)
{
	struct probing_function *p_fun;
	struct probing_function *p_fun_next = p_dev.list.head;
	
	while ((p_fun = p_fun_next) != NULL)
	{
		p_fun_next = p_fun->next;

		if (p_fun->nop_updated == 0)
		{
			if (p_fun->prev)
				p_fun->prev->next = p_fun->next;
			else
				p_dev.list.head = p_fun->next;

			if (p_fun->next)
				p_fun->next->prev = p_fun->prev;
			else
				p_dev.list.tail = p_fun->prev;

			p_dev.list.count--;

			kfree((void *) p_fun);
		}
	}

	kfree(filp->private_data);
	filp->private_data = NULL;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
	arch_atomic_set(&p_dev.used, 0);
#else
	atomic_set(&p_dev.used, 0);
#endif

	return 0;
}

ssize_t p_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
	ssize_t len = -1;
	struct probing_function *p_fun;
	struct probing_function *p_fun_next = p_dev.list.head;
	struct probing_session *p_ses = (struct probing_session *) filp->private_data;

	while ((p_fun = p_fun_next) != NULL)
	{
		p_fun_next = p_fun->next;

		if (p_fun->nop_updated == 0)
		{
			if (p_fun->prev)
				p_fun->prev->next = p_fun->next;
			else
				p_dev.list.head = p_fun->next;

			if (p_fun->next)
				p_fun->next->prev = p_fun->prev;
			else
				p_dev.list.tail = p_fun->prev;

			p_dev.list.count--;

			kfree((void *) p_fun);
		}
	}

	if (p_ses->idt_entry)
		len = binary_analysis(p_ses, buffer, length);

	return len;
}

long p_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long res = -1;
	struct probing_function *p_fun = p_dev.list.head;
	struct probing_session *p_ses = (struct probing_session *) filp->private_data;

	switch (cmd)
	{
		case P_IDT_SELECT:
			p_ses->idt_entry = get_IDT_entry((int) arg);
			if (p_ses->idt_entry)
			{
				p_ses->idt_select = (int) arg;
				p_ses->nop_select = 0;
				res = 0;
			}
			break;
		case P_NOP_SELECT:
			if (p_ses->idt_select != -1)
			{
				while (p_fun)
				{
					if (p_fun->nop_address == arg)
					{
						p_ses->nop_select = arg;
						res = 0;
						break;
					}

					p_fun = p_fun->next;
				}
			}
			break;
		case P_NOP_UPDATE:
			if (p_ses->nop_select != 0)
			{
				while (p_fun)
				{
					if (p_fun->nop_address == p_ses->nop_select)
					{
						if (p_fun->nop_updated == 0)
						{
							if (check_pf_exists(arg))
							{
								if (insert_probe_function(p_fun, arg) == 0)
								{
									p_fun->nop_updated = 1;
									res = 0;
								}
							}
						}

						break;
					}

					p_fun = p_fun->next;
				}
			}
			break;
		case P_NOP_RESTORE:
			if (p_ses->nop_select != 0)
			{
				while (p_fun)
				{
					if (p_fun->nop_address == p_ses->nop_select)
					{
						if (p_fun->nop_updated)
						{
							remove_probe_function(p_fun);
							p_fun->nop_updated = 0;
							res = 0;
						}
						
						break;
					}

					p_fun = p_fun->next;
				}
			}
			break;
		default:
			break;
	}

	return res;
}