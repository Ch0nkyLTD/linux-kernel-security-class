// SPDX-License-Identifier: GPL-2.0
/*
 * cloak_kp.c — Guardian Module (kprobe variant), Part 3
 *
 * Kretprobe on __arm64_sys_getdents64: hides PROTECTED_BASENAME
 * from directory listings unless the caller has MAGIC_GID.
 *
 * Entry handler: saves dirp via double pt_regs.
 * Return handler: filters dirent buffer, adjusts byte count.
 */

#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/dirent.h>
#include <asm/ptrace.h>
#include "guardian_kprobe.h"

struct cloak_data {
	struct linux_dirent64 __user *dirp;
};

static int cloak_entry(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    // TODO: Your implementation here
}

static int cloak_return(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    // TODO: Your implementation here
}

static struct kretprobe cloak_krp = {
	.handler       = cloak_return,
	.entry_handler = cloak_entry,
	.data_size     = sizeof(struct cloak_data),
	.maxactive     = 20,
	.kp.symbol_name = "__arm64_sys_getdents64",
};

int cloak_init(void)
{
    // TODO: Your implementation here
}

void cloak_exit(void)
{
    // TODO: Your implementation here
}
