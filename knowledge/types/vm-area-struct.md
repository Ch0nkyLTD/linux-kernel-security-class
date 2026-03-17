---
name: "vm_area_struct"
type: type
introduced_in: kernel-code-injection
introduced_date: 2026-02-27
scope: introduced
scope_history:
  - lecture: kernel-code-injection
    date: 2026-02-27
prerequisites:
  - "[[mm-struct]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/mm_types.h"
tags: [kernel, virtual-memory]
---

Describes a contiguous virtual memory region in a process. Fields: `vm_start`, `vm_end`, `vm_flags` (VM_READ, VM_WRITE, VM_EXEC), `vm_file`. Code injection finds VM_EXEC VMAs to write shellcode into, or creates VMA-less PTE mappings for stealth.

## Used In

- threadinject.c
- hidemap.c
- ptescan.c

## See Also

- [[mm-struct]]
- [[pte-construction]]
