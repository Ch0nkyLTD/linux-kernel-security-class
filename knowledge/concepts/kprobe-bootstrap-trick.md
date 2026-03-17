---
name: "kprobe bootstrap trick"
type: concept
introduced_in: kallsyms
introduced_date: 2026-02-17
scope: used
scope_history:
  - lecture: kallsyms
    date: 2026-02-17
  - lecture: ftrace
    date: 2026-02-13
  - lecture: syscall-table
    date: 2026-02-17
prerequisites:
  - "[[register-kprobe]]"
  - "[[kallsyms-lookup-name]]"
docs: ""
tags: [kernel, symbols, hooking]
---

Technique to obtain the address of `kallsyms_lookup_name` (unexported since Linux 5.7) by registering a kprobe on it. `kp.addr` is set by the kprobe subsystem during registration, revealing the function's address. After unregistering, use that address to look up any other kernel symbol.

## Used In

- syscall_hook.c
- bouncer_ftrace.c
- hidemap.c

## See Also

- [[kallsyms-lookup-name]]
- [[register-kprobe]]
