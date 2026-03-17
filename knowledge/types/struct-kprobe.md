---
name: "struct kprobe"
type: type
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: kprobes
    date: 2026-02-10
  - lecture: kallsyms
    date: 2026-02-17
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/kprobes.h"
tags: [kernel, hooking, kprobes]
---

Describes a kernel probe point. Key fields: `.symbol_name` (target function), `.pre_handler` (callback before function executes). On registration, the target instruction is replaced with BRK.

## Used In

- trace_openat.c
- bouncer.c
- secret.c
- syscall_hook.c (bootstrap)

## See Also

- [[register-kprobe]]
- [[struct-kretprobe]]
