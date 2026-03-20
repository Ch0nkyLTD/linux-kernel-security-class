/*
 * offsets.h - Kernel struct offsets for raw cred-zeroing shellcode
 *
 * Fill these in from the find_offsets module or pahole output.
 * These values are specific to your lab kernel (6.6, aarch64).
 */

#ifndef OFFSETS_H
#define OFFSETS_H

/*
 * Offset of 'cred' pointer within struct task_struct.
 * Find with: pahole -C task_struct vmlinux | grep -w cred
 * Or:        insmod find_offsets.ko && dmesg | tail
 */
#define CRED_OFFSET 0 /* TODO: fill from Exercise 1 */

/*
 * Offset of 'uid' within struct cred.
 * The uid/gid/suid/sgid/euid/egid/fsuid/fsgid fields are 8 consecutive
 * 4-byte values starting at this offset. We zero them in pairs using
 * 64-bit stores (str xzr), so this offset must be 8-byte aligned.
 */
#define UID_OFFSET 0 /* TODO: fill from Exercise 1 */

#endif /* OFFSETS_H */
