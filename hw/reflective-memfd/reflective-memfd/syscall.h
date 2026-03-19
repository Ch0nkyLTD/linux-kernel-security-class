
#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <sys/types.h>

/* ── AArch64 system call numbers ──────────────────────────────── */
#define SYS_openat       56
#define SYS_close        57
#define SYS_read         63
#define SYS_write        64
#define SYS_unlinkat     35
#define SYS_mmap        222
#define SYS_mprotect    226
#define SYS_lseek        62
#define SYS_getpid      172
#define SYS_memfd_create 279
#define SYS_exit         93

/* memfd flags (linux/memfd.h) */
#ifndef MFD_CLOEXEC
#define MFD_CLOEXEC      0x0001U
#endif

/* ── Generic syscall wrappers (inline asm, svc #0) ───────────── */

static inline long syscall0(long num) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0");
  asm volatile("svc 0" : "=r"(x0) : "r"(x8) : "memory", "cc");
  return x0;
}

static inline long syscall1(long num, long a) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  asm volatile("svc 0" : "+r"(x0) : "r"(x8) : "memory", "cc");
  return x0;
}

static inline long syscall2(long num, long a, long b) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  asm volatile("svc 0" : "+r"(x0) : "r"(x1), "r"(x8) : "memory", "cc");
  return x0;
}

static inline long syscall3(long num, long a, long b, long c) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  register long x2 asm("x2") = c;
  asm volatile("svc 0"
               : "+r"(x0)
               : "r"(x1), "r"(x2), "r"(x8)
               : "memory", "cc");
  return x0;
}

static inline long syscall4(long num, long a, long b, long c, long d) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  register long x2 asm("x2") = c;
  register long x3 asm("x3") = d;
  asm volatile("svc 0"
               : "+r"(x0)
               : "r"(x1), "r"(x2), "r"(x3), "r"(x8)
               : "memory", "cc");
  return x0;
}

static inline long syscall6(long num, long a, long b, long c, long d, long e,
                            long f) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  register long x2 asm("x2") = c;
  register long x3 asm("x3") = d;
  register long x4 asm("x4") = e;
  register long x5 asm("x5") = f;
  asm volatile("svc 0"
               : "+r"(x0)
               : "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x8)
               : "memory", "cc");
  return x0;
}

/* ── Typed wrappers ───────────────────────────────────────────── */

static inline long sys_openat(int dirfd, const char *pathname, int flags,
                              mode_t mode) {
  return syscall4(SYS_openat, (long)dirfd, (long)pathname, (long)flags,
                  (long)mode);
}

static inline long sys_close(int fd) {
  return syscall1(SYS_close, (long)fd);
}

static inline long sys_read(int fd, void *buf, size_t count) {
  return syscall3(SYS_read, (long)fd, (long)buf, (long)count);
}

static inline long sys_write(int fd, const void *buf, size_t count) {
  return syscall3(SYS_write, (long)fd, (long)buf, (long)count);
}

static inline long sys_unlinkat(int dirfd, const char *pathname, int flags) {
  return syscall3(SYS_unlinkat, (long)dirfd, (long)pathname, (long)flags);
}

static inline void *sys_mmap(void *addr, size_t length, int prot, int flags,
                             int fd, off_t offset) {
  return (void *)syscall6(SYS_mmap, (long)addr, (long)length, (long)prot,
                          (long)flags, (long)fd, (long)offset);
}

static inline long sys_mprotect(void *addr, size_t len, int prot) {
  return syscall3(SYS_mprotect, (long)addr, (long)len, (long)prot);
}

static inline off_t sys_lseek(int fd, off_t offset, int whence) {
  return syscall3(SYS_lseek, (long)fd, (long)offset, (long)whence);
}

static inline pid_t sys_getpid(void) {
  return (pid_t)syscall0(SYS_getpid);
}

static inline int sys_memfd_create(const char *name, unsigned int flags) {
  return syscall2(SYS_memfd_create, (long)name, (long)flags);
}

static inline void sys_exit(int status) {
  syscall1(SYS_exit, (long)status);
  __builtin_unreachable();
}

#endif /* SYSCALL_H */
