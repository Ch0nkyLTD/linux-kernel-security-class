---
name: "current macro"
type: concept
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: mastery
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
  - lecture: kprobes
    date: 2026-02-10
  - lecture: kernel-shellcoding
    date: 2026-03-17
prerequisites:
  - "[[task-struct]]"
docs: "https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/current.h"
tags: [kernel, process-management]
---

Per-CPU macro that returns a pointer to the currently running task's `task_struct`. On AArch64, reads `sp_el0` which the kernel stores the current task pointer in. Used everywhere: `current->cred`, `current->mm`, `current->pid`.

## Used In

- Nearly all kernel modules
- promote.md
- privesc.c

## See Also

- [[task-struct]]
- [[struct-cred]]
