---
name: "sock_release"
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

Releases a kernel socket. Must be called on every error path after `sock_create` succeeds, and during cleanup. Leaking a socket keeps the port bound until reboot.

## Used In

- kthread_beacon.ko
- capstone rootkit

## See Also

- [[sock-create]]
