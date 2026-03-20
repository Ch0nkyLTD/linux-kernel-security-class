/*
 * find_offsets.c - Print kernel struct offsets for shellcode construction
 *
 * Usage:
 *   insmod find_offsets.ko    (prints offsets, fails to load on purpose)
 *   dmesg | tail -5           (read the offsets)
 *
 * Record the values in offsets.h for use in raw_privesc.c.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cred.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Print task_struct/cred offsets for shellcode construction");

static int __init find_offsets_init(void)
{
	pr_info("=== Kernel Struct Offsets ===\n");
	pr_info("  offsetof(task_struct, cred)  = 0x%lx (%lu bytes)\n",
		(unsigned long)offsetof(struct task_struct, cred),
		(unsigned long)offsetof(struct task_struct, cred));
	pr_info("  offsetof(cred, uid)          = 0x%lx (%lu bytes)\n",
		(unsigned long)offsetof(struct cred, uid),
		(unsigned long)offsetof(struct cred, uid));
	pr_info("  offsetof(cred, gid)          = 0x%lx (%lu bytes)\n",
		(unsigned long)offsetof(struct cred, gid),
		(unsigned long)offsetof(struct cred, gid));
	pr_info("  offsetof(cred, euid)         = 0x%lx (%lu bytes)\n",
		(unsigned long)offsetof(struct cred, euid),
		(unsigned long)offsetof(struct cred, euid));
	pr_info("  sizeof(kuid_t)               = %lu\n",
		(unsigned long)sizeof(kuid_t));

	/* Return error so the module unloads immediately after printing */
	return -EINVAL;
}

module_init(find_offsets_init);
