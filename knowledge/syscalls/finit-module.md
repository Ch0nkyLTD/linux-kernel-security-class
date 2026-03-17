---
name: "finit_module"
type: syscall
introduced_in: capstone-kernel
introduced_date: 2026-02-24
scope: introduced
scope_history:
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man2/finit_module.2.html"
tags: [syscall, kernel]
---

Loads a kernel module from a file descriptor. Used by `insmod` internally. In the capstone, an alternative to `execve("/sbin/insmod")` for loading rootkit.ko after privilege escalation.

## Used In

- capstone-kernel.md

## See Also

- [[execve]]
