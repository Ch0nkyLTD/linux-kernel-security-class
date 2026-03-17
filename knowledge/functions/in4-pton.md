---
name: "in4_pton"
type: function
introduced_in: kthreads
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/net/core/utils.c"
tags: [kernel, networking]
---

Parses a dotted-quad IPv4 address string into 4 bytes in network byte order. Kernel equivalent of `inet_pton()`. Used in kthread_beacon to parse the target IP module parameter.

## Used In

- kthread_beacon.ko

## See Also

- [[sock-create]]
