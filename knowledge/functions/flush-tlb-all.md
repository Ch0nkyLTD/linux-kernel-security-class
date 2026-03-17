---
name: "flush_tlb_all"
type: function
introduced_in: syscall-table
introduced_date: 2026-02-17
scope: used
scope_history:
  - lecture: syscall-table
    date: 2026-02-17
  - lecture: virtual-memory-deepdive
    date: 2026-02-20
prerequisites:
  - "[[tlb]]"
  - "[[page-tables]]"
docs: "https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/tlbflush.h"
tags: [kernel, virtual-memory]
---

Invalidates all TLB entries across all CPUs. Required after modifying page table entries (e.g., clearing PTE_RDONLY to make the syscall table writable). Expensive but necessary for correctness.

## Used In

- syscall_hook.c (after PTE manipulation)

## See Also

- [[tlb]]
- [[pte-construction]]
