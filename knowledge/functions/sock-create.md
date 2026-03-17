---
name: "sock_create"
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
  - "[[kthread-run]]"
docs: "https://elixir.bootlin.com/linux/latest/source/net/socket.c"
tags: [kernel, networking, kthreads]
---

Kernel-space equivalent of the `socket()` syscall. Creates a `struct socket` for TCP/UDP communication from kernel context. Used in kthread-based network backdoors and beacons.

## Used In

- kthread_beacon.ko
- capstone rootkit (network backdoor)

## See Also

- [[kernel-connect]]
- [[kernel-bind]]
- [[sock-release]]
