/* SPDX-License-Identifier: GPL-2.0 */
/*
 * vuln_rw.h - Vulnerable "debug interface" driver
 *
 * Challenges 8 & 9: Arbitrary kernel read/write
 *
 * This driver exposes raw kernel memory read/write via ioctl, simulating
 * a debug interface that shipped to production without access checks.
 *
 * FOR EDUCATIONAL USE ONLY — this driver is deliberately vulnerable.
 */

#ifndef VULN_RW_H
#define VULN_RW_H

#include <linux/ioctl.h>
#include <linux/types.h>

struct vuln_rw_request {
	unsigned long kaddr;		/* kernel virtual address */
	void __user *ubuf;		/* userspace buffer */
	__kernel_size_t len;		/* byte count (max PAGE_SIZE) */
};

#define VULN_RW_MAGIC  'V'
#define VULN_KREAD     _IOR(VULN_RW_MAGIC, 1, struct vuln_rw_request)
#define VULN_KWRITE    _IOW(VULN_RW_MAGIC, 2, struct vuln_rw_request)

#endif /* VULN_RW_H */
