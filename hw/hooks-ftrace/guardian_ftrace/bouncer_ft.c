// SPDX-License-Identifier: GPL-2.0
/*
 * bouncer_ft.c — Guardian Module (ftrace variant), Part 2
 *
 * Ftrace hook on do_sys_openat2: blocks access to PROTECTED_PATH
 * unless the caller has MAGIC_GID.
 *
 * do_sys_openat2 receives args directly: x0=dfd, x1=filename, x2=how
 * No double pt_regs indirection (unlike kprobe on __arm64_sys_openat).
 *
 * Deny by zeroing fregs->regs[1] (filename) -> EFAULT.
 */

#include <linux/kernel.h>
#include <linux/ftrace.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include "guardian_ftrace.h"

static unsigned long bouncer_target_addr;

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

static void notrace bouncer_callback(unsigned long ip, unsigned long parent_ip,
				     struct ftrace_ops *op,
				     struct ftrace_regs *fregs)
{
    // TODO: Your implementation here
}

static struct ftrace_ops bouncer_ops = {
	.func  = bouncer_callback,
	/*
	 * Do NOT set FTRACE_OPS_FL_SAVE_REGS on arm64 — it requires
	 * HAVE_DYNAMIC_FTRACE_WITH_REGS which arm64 doesn't have.
	 * FL_IPMODIFY needed because we modify register state.
	 */
	.flags = FTRACE_OPS_FL_IPMODIFY |
		 FTRACE_OPS_FL_RECURSION,
};

int bouncer_init(void)
{
    // TODO: Your implementation here
}

void bouncer_exit(void)
{
    // TODO: Your implementation here
}
