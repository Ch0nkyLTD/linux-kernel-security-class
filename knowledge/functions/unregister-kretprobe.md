---
name: "unregister_kretprobe"
type: function
introduced_in: kprobes
introduced_date: 2026-02-10
scope: used
scope_history:
  - lecture: kprobes
    date: 2026-02-10
  - lecture: process-hiding
    date: 2026-03-17
prerequisites:
  - "[[register-kretprobe]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/kprobes.c"
tags: [kernel, hooking, kprobes]
---

Removes a previously registered kretprobe. Must be called during module cleanup.

## Used In

- cloak.c
- prochide.c

## See Also

- [[register-kretprobe]]
