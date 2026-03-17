---
name: "page tables"
type: concept
introduced_in: virtual-memory-deepdive
introduced_date: 2026-02-20
scope: used
scope_history:
  - lecture: virtual-memory-deepdive
    date: 2026-02-20
  - lecture: syscall-table
    date: 2026-02-17
  - lecture: kernel-code-injection
    date: 2026-02-27
prerequisites:
  - "[[virtual-memory]]"
docs: "https://developer.arm.com/documentation/den0024/latest/"
tags: [kernel, virtual-memory, aarch64]
---

AArch64 uses 4-level page tables (L0-L3) to translate virtual addresses to physical addresses. Each level indexes 9 bits of the VA. The walk: PGD (L0) -> PUD (L1) -> PMD (L2) -> PTE (L3) -> physical page. The kernel walks these to modify permissions (syscall table hooking) or create new mappings (code injection).

## Used In

- virtual-memory-deepdive.md
- syscall_hook.c (resolve_pte)
- threadinject.c

## See Also

- [[pte-construction]]
- [[tlb]]
- [[ttbr0-ttbr1-split]]
