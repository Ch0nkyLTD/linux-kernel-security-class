---
name: "struct kretprobe"
type: type
introduced_in: kprobes
introduced_date: 2026-02-10
scope: used
scope_history:
  - lecture: kprobes
    date: 2026-02-10
  - lecture: process-hiding
    date: 2026-03-17
prerequisites:
  - "[[struct-kprobe]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/kprobes.h"
tags: [kernel, hooking, kprobes]
---

Describes a return probe. Key fields: `.kp.symbol_name`, `.handler` (return callback), `.entry_handler` (entry callback), `.data_size` (per-instance private data), `.maxactive` (max concurrent instances). Uses a trampoline to intercept function returns.

## Used In

- cloak.c
- prochide.c
- capstone rootkit

## See Also

- [[register-kretprobe]]
- [[struct-kprobe]]
