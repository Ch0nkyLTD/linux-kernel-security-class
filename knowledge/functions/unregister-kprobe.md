---
name: "unregister_kprobe"
type: function
introduced_in: kprobes
introduced_date: 2026-02-10
scope: introduced
scope_history:
  - lecture: kprobes
    date: 2026-02-10
prerequisites:
  - "[[register-kprobe]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/kprobes.c"
tags: [kernel, hooking, kprobes]
---

Removes a previously registered kprobe, restoring the original instruction at the probe point. Must be called in module_exit to avoid dangling hooks.

## Used In

- trace_openat.c
- bouncer.c
- secret.c

## See Also

- [[register-kprobe]]
