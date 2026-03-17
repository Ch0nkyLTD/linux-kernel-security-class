---
name: "set_pte"
type: function
introduced_in: kernel-code-injection
introduced_date: 2026-02-27
scope: used
scope_history:
  - lecture: kernel-code-injection
    date: 2026-02-27
  - lecture: syscall-table
    date: 2026-02-17
prerequisites:
  - "[[page-tables]]"
  - "[[pte-construction]]"
docs: "https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/pgtable.h"
tags: [kernel, virtual-memory, injection]
---

Writes a PTE value into a page table entry. Used to install custom mappings (e.g., making the syscall table writable, or creating VMA-less mappings for stealthy code injection).

## Used In

- syscall_hook.c
- threadinject.c

## See Also

- [[flush-tlb-all]]
- [[page-tables]]
