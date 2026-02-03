#ifndef SYSCALL_UTILS_H
#define SYSCALL_UTILS_H

/* Feature test macros must come first */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/*
 * ASSIGNMENT: Implement these syscall wrappers for AArch64
 *
 * Your task is to:
 * 1. Look up the AArch64 syscall numbers for each syscall
 * 2. Implement the syscall0-syscall6 wrappers using inline assembly
 * 3. Implement each syscall wrapper function
 *
 * Use the pattern: svc #0 instruction with proper register setup
 * Syscall number goes in x8, arguments in x0-x5, return value in x0
 */

/* =============================================================================
 * PART 1: SYSCALL WRAPPERS (IMPLEMENT THESE)
 * =============================================================================
 */

/* TODO: Implement syscall wrappers for 0-6 arguments */
static inline long syscall0(long num) {
    // Your solution here!
}

static inline long syscall1(long num, long arg1) {
    // Your solution here!
}

static inline long syscall2(long num, long arg1, long arg2) {
    // Your solution here!
}

static inline long syscall3(long num, long arg1, long arg2, long arg3) {
    // Your solution here!
}

static inline long syscall4(long num, long arg1, long arg2, long arg3,
                            long arg4) {
    // Your solution here!
}

static inline long syscall5(long num, long arg1, long arg2, long arg3,
                            long arg4, long arg5) {
    // Your solution here!
}

static inline long syscall6(long num, long arg1, long arg2, long arg3,
                            long arg4, long arg5, long arg6) {
    // Your solution here!
}

/* =============================================================================
 * PART 2: SYSCALL NUMBERS (LOOK THESE UP FOR AARCH64)
 * =============================================================================
 */

/* TODO: Look up AArch64 syscall numbers and define them here */
/* Hint: Check arch/arm64/include/asm/unistd32.h or use `ausyscall --dump` */

// File operations
    // Your solution here!

/* =============================================================================
 * PART 3: SYSCALL WRAPPER FUNCTIONS (IMPLEMENT THESE)
 * =============================================================================
 */

/* File descriptor constants */
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

/* Open flags */
#ifndef O_RDONLY
#define O_RDONLY 0x00
#define O_WRONLY 0x01
#define O_RDWR 0x02
#define O_CREAT 0x40
#define O_TRUNC 0x200
#define O_APPEND 0x400
#endif

/* Memory protection flags */
#ifndef PROT_READ
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#endif

/* Memory mapping flags */
#ifndef MAP_SHARED
#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20
#endif

/* TODO: Implement these syscall wrapper functions */

static inline int sys_openat(int dirfd, const char *pathname, int flags,
                             mode_t mode) {
    // Your solution here!
}

static inline int sys_close(int fd) {
    // Your solution here!
}

static inline ssize_t sys_read(int fd, void *buf, size_t count) {
    // Your solution here!
}

static inline ssize_t sys_write(int fd, const void *buf, size_t count) {
    // Your solution here!
}

static inline ssize_t sys_pread64(int fd, void *buf, size_t count,
                                  off_t offset) {
    // Your solution here!
}

static inline ssize_t sys_pwrite64(int fd, const void *buf, size_t count,
                                   off_t offset) {
    // Your solution here!
}

static inline off_t sys_lseek(int fd, off_t offset, int whence) {
    // Your solution here!
}

static inline int sys_fstat(int fd, struct stat *statbuf) {
    // Your solution here!
}

static inline void *sys_mmap(void *addr, size_t length, int prot, int flags,
                             int fd, off_t offset) {
    // Your solution here!
}

static inline int sys_munmap(void *addr, size_t length) {
    // Your solution here!
}

static inline int sys_mprotect(void *addr, size_t len, int prot) {
    // Your solution here!
}

static inline void sys_exit(int status) {
    // Your solution here!
}

static inline int sys_utimensat(int dirfd, const char *pathname,
                                const void *times, int flags) {
    // Your solution here!
}

/* Note: C library functions like strlen, memcpy, etc. are implemented in minc.c
 * Students must implement these in the separate minimal C library.
 */

#endif /* SYSCALL_UTILS_H */
