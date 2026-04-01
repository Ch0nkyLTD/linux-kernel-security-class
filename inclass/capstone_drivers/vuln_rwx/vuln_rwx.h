/* SPDX-License-Identifier: GPL-2.0 */
/*
 * vuln_rwx.h - Vulnerable "JIT engine" driver interface
 *
 * Challenge 7: RWX Kernel Shellcode
 *
 * This driver intentionally allocates executable kernel memory and runs
 * user-supplied code in ring 0. It simulates a buggy JIT compiler that
 * fails to validate its input.
 *
 * FOR EDUCATIONAL USE ONLY — this driver is deliberately vulnerable.
 */

#ifndef VULN_RWX_H
#define VULN_RWX_H

#include <linux/ioctl.h>
#include <linux/types.h>

struct vuln_rwx_request {
	void __user *code;	/* pointer to shellcode buffer */
	__kernel_size_t len;	/* shellcode length (max PAGE_SIZE) */
};

#define VULN_RWX_MAGIC 'R'
#define VULN_RWX_EXEC  _IOW(VULN_RWX_MAGIC, 1, struct vuln_rwx_request)

#endif /* VULN_RWX_H */
