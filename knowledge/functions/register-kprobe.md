---
name: "register_kprobe"
type: function
introduced_in: kprobes
introduced_date: 2026-02-10
scope: mastery
scope_history:
  - lecture: kprobes
    date: 2026-02-10
  - lecture: ftrace
    date: 2026-02-13
  - lecture: syscall-table
    date: 2026-02-17
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[struct-kprobe]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/kprobes.c"
tags: [kernel, hooking, kprobes]
---

Registers a kprobe at the address or symbol specified in the `struct kprobe`. On AArch64, replaces the target instruction with a BRK, causing a debug exception that routes to the pre_handler callback. Core hooking mechanism for the course.

## Used In

- trace_openat.c
- bouncer.c
- secret.c
- magic_backdoor.c
- syscall_hook.c (bootstrap)

## See Also

- [[unregister-kprobe]]
- [[register-kretprobe]]
- [[struct-kprobe]]
