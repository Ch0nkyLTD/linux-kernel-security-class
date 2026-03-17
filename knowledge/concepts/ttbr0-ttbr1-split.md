---
name: "TTBR0/TTBR1 split"
type: concept
introduced_in: virtual-memory-deepdive
introduced_date: 2026-02-20
scope: introduced
scope_history:
  - lecture: virtual-memory-deepdive
    date: 2026-02-20
prerequisites:
  - "[[page-tables]]"
docs: ""
tags: [kernel, virtual-memory, aarch64]
---

AArch64 uses two translation table base registers: TTBR0_EL1 for the lower VA range (userspace, addresses starting with 0x0000...) and TTBR1_EL1 for the upper VA range (kernel, addresses starting with 0xFFFF...). On context switch, only TTBR0 changes; TTBR1 stays the same (kernel mappings are global).

## Used In

- virtual-memory-deepdive.md

## See Also

- [[page-tables]]
- [[tlb]]
