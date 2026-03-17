---
name: "kernel_bind"
type: function
introduced_in: kthreads
introduced_date: 2026-03-17
scope: used
scope_history:
  - lecture: kthreads
    date: 2026-03-17
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - "[[sock-create]]"
docs: "https://elixir.bootlin.com/linux/latest/source/net/socket.c"
tags: [kernel, networking]
---

Kernel-space equivalent of `bind()`. Binds a kernel socket to a local address and port for listening.

## Used In

- capstone rootkit (network backdoor listener)

## See Also

- [[kernel-listen]]
- [[kernel-accept]]
