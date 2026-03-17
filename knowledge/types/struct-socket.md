---
name: "struct socket"
type: type
introduced_in: kthreads
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/net.h"
tags: [kernel, networking]
---

Kernel-space socket object, analogous to a userland socket file descriptor. Created by `sock_create()`, used with `kernel_connect/bind/listen/accept/sendmsg/recvmsg`, released by `sock_release()`.

## Used In

- kthread_beacon.ko
- capstone rootkit (network backdoor)

## See Also

- [[sock-create]]
- [[kernel-connect]]
