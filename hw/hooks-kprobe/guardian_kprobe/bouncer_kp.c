// SPDX-License-Identifier: GPL-2.0
/*
 * bouncer_kp.c — Guardian Module (kprobe variant), Part 2
 *
 * Kprobe on __arm64_sys_openat: blocks access to PROTECTED_PATH
 * unless the caller has MAGIC_GID.
 *
 * AArch64 double pt_regs:
 *   regs->regs[0] -> user pt_regs
 *   user_regs->regs[1] -> filename
 * Deny by zeroing user_regs->regs[1] -> EFAULT
 */

#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <asm/ptrace.h>
#include "guardian_kprobe.h"

static int bouncer_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
    // TODO: Your implementation here
}

static struct kprobe bouncer_kp = {
	.symbol_name = "__arm64_sys_openat",
	.pre_handler = bouncer_pre_handler,
};

int bouncer_init(void)
{
    // TODO: Your implementation here
}

void bouncer_exit(void)
{
    // TODO: Your implementation here
}
