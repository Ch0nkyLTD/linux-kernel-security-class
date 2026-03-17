---
name: "kernel_listen"
type: function
introduced_in: kthreads
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - "[[kernel-bind]]"
docs: "https://elixir.bootlin.com/linux/latest/source/net/socket.c"
tags: [kernel, networking]
---

Kernel-space equivalent of `listen()`. Marks a bound socket as passive, ready to accept connections.

## Used In

- capstone rootkit (network backdoor)

## See Also

- [[kernel-bind]]
- [[kernel-accept]]
