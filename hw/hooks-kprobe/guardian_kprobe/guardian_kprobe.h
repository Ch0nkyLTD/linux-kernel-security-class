/* SPDX-License-Identifier: GPL-2.0 */
#ifndef GUARDIAN_KPROBE_H
#define GUARDIAN_KPROBE_H

#include <linux/types.h>
#include <linux/sched.h>

/* --- Configuration --- */
#define PROTECTED_PATH     "/tmp/secret"
#define PROTECTED_BASENAME "secret"
#define MAGIC_GID          1337
#define MAGIC_BYTES        0xDEADBEEF
#define DEVICE_NAME        "guardian"
#define CLASS_NAME         "guardian_class"
#define EVENT_LOG_SIZE     64
#define MAX_PATH_LEN       256
#define MAX_BUF_SIZE       (1 << 16)

/* --- Event log entry --- */
struct guardian_event {
	u64       timestamp;
	pid_t     pid;
	uid_t     uid;
	bool      allowed;
	char      path[64];
	char      comm[TASK_COMM_LEN];
};

/* --- Shared helpers --- */
bool guardian_has_magic_gid(void);
void guardian_log_event(const char *path, bool allowed);

/* --- Sub-module init/exit --- */
int  bouncer_init(void);
void bouncer_exit(void);
int  cloak_init(void);
void cloak_exit(void);

#endif /* GUARDIAN_KPROBE_H */
