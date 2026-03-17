---
name: "vm_mmap"
type: function
introduced_in: kernel-code-injection
introduced_date: 2026-02-27
scope: introduced
scope_history:
  - lecture: kernel-code-injection
    date: 2026-02-27
prerequisites:
  - "[[vm-area-struct]]"
  - "[[mm-struct]]"
docs: "https://elixir.bootlin.com/linux/latest/source/mm/util.c"
tags: [kernel, memory, injection]
---

Creates a memory mapping in a process's address space from kernel context. Kernel-internal equivalent of the mmap syscall. Used in injection to create executable pages in a target process.

## Used In

- threadinject.c (alternative approach)
- kernel-code-injection.md

## See Also

- [[mm-struct]]
- [[vm-area-struct]]
