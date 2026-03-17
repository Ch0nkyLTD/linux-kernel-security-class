---
name: "strace"
type: tool
introduced_in: linux_file_io
introduced_date: 2026-01-20
scope: used
scope_history:
  - lecture: linux_file_io
    date: 2026-01-20
  - lecture: linux_filesystem_ops
    date: 2026-01-30
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man1/strace.1.html"
tags: [tool, debugging]
---

Traces system calls made by a process using ptrace. Shows syscall name, arguments, and return values. Used in the course to understand how `ls`, `ps`, and other tools interact with the kernel, revealing which syscalls to hook for hiding.

## Used In

- linux_file_io.md
- linux_filesystem_ops.md

## See Also

- [[openat]]
- [[getdents64]]
