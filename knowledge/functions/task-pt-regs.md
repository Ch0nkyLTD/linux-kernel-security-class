---
name: "task_pt_regs"
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
  - "[[struct-pt-regs]]"
docs: "https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/processor.h"
tags: [kernel, injection]
---

Returns a pointer to the saved `pt_regs` at the top of a task's kernel stack. Used to read or modify a target process's register state (e.g., redirect PC for code injection).

## Used In

- threadinject.c
- capstone rootkit

## See Also

- [[access-process-vm]]
- [[struct-pt-regs]]
