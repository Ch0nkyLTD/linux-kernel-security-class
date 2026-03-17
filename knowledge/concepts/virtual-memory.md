---
name: "virtual memory"
type: concept
introduced_in: linux-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: linux-basics
    date: 2026-01-16
  - lecture: virtual-memory-deepdive
    date: 2026-02-20
  - lecture: kernel-code-injection
    date: 2026-02-27
prerequisites:
  - (none)
docs: ""
tags: [kernel, virtual-memory]
---

Abstraction that gives each process its own private address space. The MMU translates virtual addresses to physical addresses using page tables. Enables isolation, memory protection, demand paging, and shared memory. On AArch64, the VA space is split between userspace (TTBR0) and kernel (TTBR1).

## Used In

- linux-basics.md
- virtual-memory-deepdive.md

## See Also

- [[page-tables]]
- [[ttbr0-ttbr1-split]]
- [[mm-struct]]
