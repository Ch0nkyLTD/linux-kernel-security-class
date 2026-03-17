---
name: "openat"
type: syscall
introduced_in: linux_file_io
introduced_date: 2026-01-20
scope: used
scope_history:
  - lecture: linux_file_io
    date: 2026-01-20
  - lecture: kprobes
    date: 2026-02-10
  - lecture: ftrace
    date: 2026-02-13
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man2/openat.2.html"
tags: [syscall, filesystem]
---

Opens a file relative to a directory fd (AT_FDCWD for cwd). Primary target for kprobe/ftrace hooking demonstrations in the course. Arguments: `(int dirfd, const char *pathname, int flags, mode_t mode)`. The double-pt_regs indirection is demonstrated by intercepting this syscall.

## Used In

- trace_openat.c
- trace_openat_ftrace.c
- bouncer.c
- bouncer_ftrace.c

## See Also

- [[getdents64]]
- [[read-syscall]]
- [[write-syscall]]
