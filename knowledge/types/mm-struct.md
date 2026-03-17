---
name: "mm_struct"
type: type
introduced_in: kernel-code-injection
introduced_date: 2026-02-27
scope: introduced
scope_history:
  - lecture: kernel-code-injection
    date: 2026-02-27
prerequisites:
  - "[[task-struct]]"
  - "[[page-tables]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/mm_types.h"
tags: [kernel, virtual-memory]
---

Memory descriptor for a process. Contains the PGD pointer (`pgd`), VMA tree (maple_tree), and mmap_lock. Kthreads have `mm = NULL`. Central to code injection: the attacker walks the target's mm to find executable VMAs or construct new PTEs.

## Used In

- threadinject.c
- kernel-code-injection.md

## See Also

- [[vm-area-struct]]
- [[task-struct]]
- [[page-tables]]
