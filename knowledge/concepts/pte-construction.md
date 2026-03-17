---
name: "PTE construction"
type: concept
introduced_in: syscall-table
introduced_date: 2026-02-17
scope: used
scope_history:
  - lecture: syscall-table
    date: 2026-02-17
  - lecture: kernel-code-injection
    date: 2026-02-27
prerequisites:
  - "[[page-tables]]"
docs: ""
tags: [kernel, virtual-memory, injection]
---

Building page table entries from scratch to create arbitrary virtual-to-physical mappings. Uses `alloc_page()`, `page_to_pfn()`, `pfn_pte()`, and `set_pte()`. In syscall table hooking, modifies existing PTEs to clear PTE_RDONLY. In code injection, creates VMA-less mappings invisible to /proc/pid/maps.

## Used In

- syscall_hook.c
- threadinject.c

## See Also

- [[page-tables]]
- [[set-pte]]
- [[flush-tlb-all]]
