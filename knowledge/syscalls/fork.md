---
name: "fork"
type: syscall
introduced_in: linux-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: linux-basics
    date: 2026-01-16
  - lecture: elf-in-depth-and-process-creation
    date: 2026-01-30
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man2/fork.2.html"
tags: [syscall, process-management]
---

Creates a new process by duplicating the calling process. The child gets a copy of the parent's address space (COW). Returns 0 in the child, child's PID in the parent. Used with `execve()` for the standard process creation pattern.

## Used In

- linux-basics.md
- elf-in-depth-and-process-creation.md

## See Also

- [[execve]]
- [[clone]]
