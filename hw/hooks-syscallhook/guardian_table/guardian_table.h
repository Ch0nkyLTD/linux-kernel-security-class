/* SPDX-License-Identifier: GPL-2.0 */
#ifndef GUARDIAN_TABLE_H
#define GUARDIAN_TABLE_H

#include <asm/ptrace.h>
#include <linux/sched.h>
#include <linux/types.h>

/* --- Configuration --- */
#define PROTECTED_PATH "/tmp/secret"
#define PROTECTED_BASENAME "secret"
#define MAGIC_GID 1337
#define MAGIC_BYTES 0xDEADBEEF
#define DEVICE_NAME "guardian"
#define CLASS_NAME "guardian_class"
#define EVENT_LOG_SIZE 64
#define MAX_PATH_LEN 256
#define MAX_BUF_SIZE (1 << 16)

/* --- Syscall function type --- */
typedef long (*syscall_fn_t)(const struct pt_regs *);

/* --- Event log entry --- */
struct guardian_event {
  u64 timestamp;
  pid_t pid;
  uid_t uid;
  bool allowed;
  char path[64];
  char comm[TASK_COMM_LEN];
};

/* --- Shared state (defined in guardian_table_main.c) --- */
extern syscall_fn_t *sys_call_table_ptr;
extern syscall_fn_t original_openat;
extern syscall_fn_t original_getdents64;

/* --- Shared helpers --- */
bool guardian_has_magic_gid(void);
void guardian_log_event(const char *path, bool allowed);

/* --- Sub-module hooks --- */
syscall_fn_t bouncer_get_hook(void);
syscall_fn_t cloak_get_hook(void);

#endif /* GUARDIAN_TABLE_H */
