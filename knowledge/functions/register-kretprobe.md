---
name: "register_kretprobe"
type: function
introduced_in: kprobes
introduced_date: 2026-02-10
scope: mastery
scope_history:
  - lecture: kprobes
    date: 2026-02-10
  - lecture: process-hiding
    date: 2026-03-17
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[struct-kretprobe]]"
  - "[[register-kprobe]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/kprobes.c"
tags: [kernel, hooking, kprobes]
---

Registers a return probe that fires both on function entry (entry_handler) and on function return (handler). Uses a trampoline to intercept the return path. Essential for post-call filtering (e.g., modifying getdents64 results).

## Used In

- cloak.c
- prochide.c
- capstone rootkit (file hiding, process hiding)

## See Also

- [[unregister-kretprobe]]
- [[struct-kretprobe]]
- [[register-kprobe]]
