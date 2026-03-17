---
name: "TLB"
type: concept
introduced_in: virtual-memory-deepdive
introduced_date: 2026-02-20
scope: used
scope_history:
  - lecture: virtual-memory-deepdive
    date: 2026-02-20
  - lecture: syscall-table
    date: 2026-02-17
prerequisites:
  - "[[page-tables]]"
docs: ""
tags: [kernel, virtual-memory, aarch64]
---

Translation Lookaside Buffer -- hardware cache of recent virtual-to-physical translations. AArch64 has L1 micro-TLB and L2 main TLB. After modifying PTEs, the TLB must be invalidated (TLBI instructions or `flush_tlb_all()`) or the CPU may use stale translations. ASID tags allow per-process TLB entries to coexist.

## Used In

- virtual-memory-deepdive.md
- syscall_hook.c

## See Also

- [[page-tables]]
- [[flush-tlb-all]]
