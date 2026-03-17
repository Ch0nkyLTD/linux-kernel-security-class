---
name: "insmod"
type: tool
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man8/insmod.8.html"
tags: [tool, kernel]
---

Loads a kernel module (.ko file) into the running kernel via the `finit_module()` syscall. Requires root privileges. The entry point is the function registered with `module_init()`. In the capstone, this is how the rootkit gets loaded after privilege escalation.

## Used In

- ko-basics.md
- capstone chain (Stage 3)

## See Also

- [[rmmod]]
- [[lsmod]]
