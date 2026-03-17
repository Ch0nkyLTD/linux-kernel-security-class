---
name: "fexecve"
type: syscall
introduced_in: reflective-payloads
introduced_date: 2026-03-13
scope: used
scope_history:
  - lecture: reflective-payloads
    date: 2026-03-13
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - "[[memfd-create]]"
docs: "https://man7.org/linux/man-pages/man3/fexecve.3.html"
tags: [syscall, stealth]
---

Executes a program referred to by a file descriptor rather than a path. Combined with `memfd_create()`, runs an ELF binary that exists only in memory. Implemented as `execveat(fd, "", argv, envp, AT_EMPTY_PATH)`.

## Used In

- reflective-payloads.md
- capstone Part A (memfd ELF loader)

## See Also

- [[memfd-create]]
- [[execve]]
