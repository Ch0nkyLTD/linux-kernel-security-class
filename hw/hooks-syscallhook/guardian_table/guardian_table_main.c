// SPDX-License-Identifier: GPL-2.0
/*
 * guardian_table_main.c — Guardian Module (syscall table variant)
 *
 * Part 1: Character device + kfifo + magic GID + PTE utilities + symbol resolution
 *
 * Patches sys_call_table[] directly — the kernel equivalent of GOT/PLT
 * patching in userland. Uses a single read-write window for both the
 * openat and getdents64 hooks.
 *
 * PTE manipulation:
 *   sys_call_table is in .rodata (CONFIG_STRICT_KERNEL_RWX).
 *   We walk the kernel page tables to find the PTE/PMD mapping the table,
 *   clear PTE_RDONLY, modify entries, then restore read-only protection.
 *
 * WARNING: Educational use only.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/kfifo.h>
#include <linux/spinlock.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/pgtable.h>
#include <asm/ptrace.h>
#include <asm/tlbflush.h>
#include <asm/unistd.h>
#include "guardian_table.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Course Instructor");
MODULE_DESCRIPTION("Guardian (syscall table) — directory protector via table patching");
MODULE_VERSION("1.0");

/* --- Private state — accessed only through helper functions --- */
DEFINE_KFIFO(event_log, struct guardian_event, EVENT_LOG_SIZE);
DEFINE_SPINLOCK(event_lock);

/* --- Shared state — referenced by bouncer_tbl.c and cloak_tbl.c --- */
syscall_fn_t *sys_call_table_ptr;
syscall_fn_t original_openat;
syscall_fn_t original_getdents64;

/* --- Private state --- */
static dev_t          dev_num;
static struct cdev    my_cdev;
static struct class   *dev_class;
static struct device  *dev_device;

/* PTE state */
static pte_t  saved_pte;
static pte_t *saved_ptep;
static struct mm_struct *p_init_mm;

typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);

/* --- Symbol resolution --- */

static unsigned long kprobe_lookup(const char *name)
{
	struct kprobe kp = { .symbol_name = name };
	unsigned long addr;

	if (register_kprobe(&kp) < 0)
		return 0;
	addr = (unsigned long)kp.addr;
	unregister_kprobe(&kp);
	return addr;
}

/* --- PTE manipulation --- */

static pte_t *resolve_pte(unsigned long addr, unsigned int *levelp)
{
    // TODO: Your implementation here
}

static int set_table_rw(void)
{
    // TODO: Your implementation here
}

static void set_table_ro(void)
{
    // TODO: Your implementation here
}

/* --- Shared helpers --- */

bool guardian_has_magic_gid(void)
{
    // TODO: Your implementation here
}

void guardian_log_event(const char *path, bool allowed)
{
    // TODO: Your implementation here
}

/* --- Credential modification --- */

static int grant_magic_gid(void)
{
    // TODO: Your implementation here
}

/* --- Character device ops --- */

static int guardian_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int guardian_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t guardian_write(struct file *file, const char __user *buf,
			      size_t count, loff_t *ppos)
{
    // TODO: Your implementation here
}

static ssize_t guardian_read(struct file *file, char __user *buf,
			     size_t count, loff_t *ppos)
{
    // TODO: Your implementation here
}

static const struct file_operations guardian_fops = {
	.owner   = THIS_MODULE,
	.open    = guardian_open,
	.release = guardian_release,
	.read    = guardian_read,
	.write   = guardian_write,
};

/* --- Module init / exit --- */

static int __init guardian_init(void)
{
    // TODO: Your implementation here
}

static void __exit guardian_exit(void)
{
    // TODO: Your implementation here
}

module_init(guardian_init);
module_exit(guardian_exit);
