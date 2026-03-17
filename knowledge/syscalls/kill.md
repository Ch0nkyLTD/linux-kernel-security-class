---
name: "kill"
type: syscall
introduced_in: promote
introduced_date: 2026-02-10
scope: used
scope_history:
  - lecture: promote
    date: 2026-02-10
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man2/kill.2.html"
tags: [syscall, signals]
---

Sends a signal to a process. In the capstone, repurposed as a covert C2 channel: a kprobe on the kill syscall intercepts signal 62, interpreting the target PID as an encoded command. Normal signals pass through unmodified.

## Used In

- magic_backdoor.c
- full-chain-rootkit.md

## See Also

- (none)
