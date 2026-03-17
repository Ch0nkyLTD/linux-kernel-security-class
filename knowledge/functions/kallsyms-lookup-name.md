---
name: "kallsyms_lookup_name"
type: function
introduced_in: kallsyms
introduced_date: 2026-02-17
scope: used
scope_history:
  - lecture: kallsyms
    date: 2026-02-17
  - lecture: syscall-table
    date: 2026-02-17
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/kallsyms.c"
tags: [kernel, symbols]
---

Resolves a kernel symbol name to its virtual address. Was exported before Linux 5.7; now unexported, requiring the kprobe bootstrap trick to obtain its address first. Central to all hooking techniques that need to find unexported symbols.

## Used In

- syscall_hook.c (via kprobe trick)
- kallsyms.md

## See Also

- [[kprobe-bootstrap-trick]]
- [[proc-kallsyms]]
