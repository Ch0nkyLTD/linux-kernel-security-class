---
name: "kernel_accept"
type: function
introduced_in: kthreads
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - "[[kernel-listen]]"
docs: "https://elixir.bootlin.com/linux/latest/source/net/socket.c"
tags: [kernel, networking]
---

Kernel-space equivalent of `accept()`. Blocks until a connection arrives, then returns a new `struct socket` for the client.

## Used In

- capstone rootkit (network backdoor)

## See Also

- [[kernel-listen]]
- [[kernel-recvmsg]]
