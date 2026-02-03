/* plt_hook.c - PLT stub patching hook for protected files */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <link.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define TARGET_OPENAT "openat"
#define TARGET_UNLINKAT "unlinkat"
#define PROTECTED_DIR "/tmp/protected"

/* Function pointer types for the original functions */
typedef int (*orig_openat_type)(int dirfd, const char *pathname, int flags,
                                mode_t mode);
typedef int (*orig_unlinkat_type)(int dirfd, const char *pathname, int flags);
static orig_openat_type real_openat = NULL;
static orig_unlinkat_type real_unlinkat = NULL;

int hacked_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
  int result = -1;
    // Your solution here!
  return result;
}

int hacked_unlinkat(int dirfd, const char *pathname, int flags) {
  int result = -1;
    // Your solution here!
  return result;
}

#define PAGE_START(x) ((x) & ~(getpagesize() - 1))

static int read_exact(int fd, void *buf, size_t size, off_t offset) {
  size_t done = 0;
  while (done < size) {
    ssize_t n = pread(fd, (char *)buf + done, size - done, offset + done);
    if (n <= 0) {
      return -1;
    }
    done += (size_t)n;
  }
  return 0;
}

static int find_plt_section(const char *path, Elf64_Addr base_addr,
                            Elf64_Addr *plt_addr, size_t *plt_size,
                            int *plt_has_plt0) {
  int status = -1;
    // Your solution here!
  return status;
}

static uint32_t encode_ldr_literal_x16(void) {
    // Your solution here!
}

static uint32_t encode_br_x16(void) {
    // Your solution here!
}

static int patch_plt_entry(void *entry, void *target) {
  int status = -1;
    // Your solution here!
  return status;
}

static int callback(struct dl_phdr_info *info, size_t size, void *data) {
  int ret = 0;
    // Your solution here!
  return ret;
}

__attribute__((constructor)) void install_hooks(void) {
    // Your solution here!
}
