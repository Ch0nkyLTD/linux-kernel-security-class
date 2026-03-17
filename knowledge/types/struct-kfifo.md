---
name: "struct kfifo"
type: type
introduced_in: promote
introduced_date: 2026-02-10
scope: introduced
scope_history:
  - lecture: promote
    date: 2026-02-10
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/kfifo.h"
tags: [kernel, data-structures]
---

Lock-free circular buffer for single-producer/single-consumer scenarios. Used in chardev modules to buffer data between kernel event handlers and userspace readers (e.g., logging intercepted syscalls).

## Used In

- bouncer.c
- magic_backdoor.c

## See Also

- [[struct-file-operations]]
