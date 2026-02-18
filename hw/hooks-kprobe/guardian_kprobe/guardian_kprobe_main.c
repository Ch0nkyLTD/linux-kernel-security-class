// SPDX-License-Identifier: GPL-2.0
/*
 * guardian_kprobe_main.c — Guardian Module (kprobe variant)
 *
 * Part 1: Character device + kfifo event log + magic GID credential modification
 *
 * Write 0xDEADBEEF to /dev/guardian to grant GID 1337 (bypass).
 * Read /dev/guardian to drain the event log.
 * Hooks are in bouncer_kp.c (Part 2) and cloak_kp.c (Part 3).
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
#include <asm/ptrace.h>
#include "guardian_kprobe.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Course Instructor");
MODULE_DESCRIPTION("Guardian (kprobe) — directory protector with magic GID bypass");
MODULE_VERSION("1.0");

/* --- Private state — accessed only through helper functions --- */
DEFINE_KFIFO(event_log, struct guardian_event, EVENT_LOG_SIZE);
DEFINE_SPINLOCK(event_lock);

/* --- Private state --- */
static dev_t          dev_num;
static struct cdev    my_cdev;
static struct class   *dev_class;
static struct device  *dev_device;

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
