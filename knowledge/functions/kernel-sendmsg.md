---
name: "kernel_sendmsg"
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

Sends data over a kernel socket using `struct kvec` and `struct msghdr`. Kernel-space equivalent of `sendmsg()`.

## Used In

- kthread_beacon.ko
- capstone rootkit

## See Also

- [[kernel-recvmsg]]
- [[sock-create]]
