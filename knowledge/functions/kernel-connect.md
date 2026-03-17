---
name: "kernel_connect"
type: function
introduced_in: kthreads
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - "[[sock-create]]"
docs: "https://elixir.bootlin.com/linux/latest/source/net/socket.c"
tags: [kernel, networking]
---

Kernel-space equivalent of `connect()`. Establishes a TCP connection from a kernel socket to a remote address.

## Used In

- kthread_beacon.ko (connect-back beacon)

## See Also

- [[sock-create]]
- [[kernel-sendmsg]]
