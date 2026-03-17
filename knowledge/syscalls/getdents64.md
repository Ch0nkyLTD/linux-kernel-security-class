---
name: "getdents64"
type: syscall
introduced_in: linux_filesystem_ops
introduced_date: 2026-01-30
scope: mastery
scope_history:
  - lecture: linux_filesystem_ops
    date: 2026-01-30
  - lecture: kprobes
    date: 2026-02-10
  - lecture: process-hiding
    date: 2026-03-17
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - "[[openat]]"
docs: "https://man7.org/linux/man-pages/man2/getdents64.2.html"
tags: [syscall, filesystem, stealth]
---

Reads directory entries into a buffer of `linux_dirent64` structs. The core syscall targeted by file hiding and process hiding rootkit techniques. Hooked via kretprobe to filter entries before they reach userspace.

## Used In

- cloak.c
- prochide.c
- capstone rootkit

## See Also

- [[struct-linux-dirent64]]
- [[file-hiding]]
- [[process-hiding-concept]]
