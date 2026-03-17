---
name: "clone"
type: syscall
introduced_in: kernel-code-injection
introduced_date: 2026-02-27
scope: introduced
scope_history:
  - lecture: kernel-code-injection
    date: 2026-02-27
prerequisites:
  - "[[fork]]"
docs: "https://man7.org/linux/man-pages/man2/clone.2.html"
tags: [syscall, process-management]
---

Creates a new thread or process with fine-grained sharing control. Flags: CLONE_VM (share address space), CLONE_THREAD (same thread group), CLONE_SIGHAND (share signal handlers). `kernel_clone()` is the in-kernel version used by threadinject.c to spawn an injected thread in the target process.

## Used In

- kernel-code-injection.md
- threadinject.c

## See Also

- [[fork]]
