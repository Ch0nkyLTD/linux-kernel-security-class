---
name: "access_process_vm"
type: function
introduced_in: kernel-code-injection
introduced_date: 2026-02-27
scope: used
scope_history:
  - lecture: kernel-code-injection
    date: 2026-02-27
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - "[[task-struct]]"
  - "[[vm-area-struct]]"
docs: "https://elixir.bootlin.com/linux/latest/source/mm/memory.c"
tags: [kernel, injection, memory]
---

Reads or writes another process's virtual memory from kernel context. Uses FOLL_WRITE|FOLL_FORCE flags to write to executable pages. Simpler alternative to manual PTE construction for code injection.

## Used In

- threadinject.c (simple mode)
- capstone rootkit (code injection)

## See Also

- [[task-pt-regs]]
- [[vm-area-struct]]
- [[pte-construction]]
