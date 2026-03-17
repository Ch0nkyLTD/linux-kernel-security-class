---
name: "kernel_recvmsg"
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

Receives data from a kernel socket. Kernel-space equivalent of `recvmsg()`.

## Used In

- capstone rootkit (backdoor command parsing)

## See Also

- [[kernel-sendmsg]]
- [[sock-create]]
