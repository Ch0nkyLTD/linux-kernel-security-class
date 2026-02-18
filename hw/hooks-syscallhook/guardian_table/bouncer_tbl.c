// SPDX-License-Identifier: GPL-2.0
/*
 * bouncer_tbl.c — Guardian Module (syscall table variant), Part 2
 *
 * Replacement handler for sys_call_table[__NR_openat].
 * Blocks access to PROTECTED_PATH unless the caller has MAGIC_GID.
 *
 * Direct pt_regs: regs->regs[1] = filename (no double indirection).
 * Returns -EACCES directly (cleaner than zeroing regs).
 */

#include "guardian_table.h"
#include <asm/ptrace.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

static long hooked_openat(const struct pt_regs *regs) {
  // TODO: Your implementation here
}

syscall_fn_t bouncer_get_hook(void) { return hooked_openat; }
