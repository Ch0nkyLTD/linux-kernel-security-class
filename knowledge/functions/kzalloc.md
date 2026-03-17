---
name: "kzalloc"
type: function
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
prerequisites:
  - "[[kmalloc]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/slab.h"
tags: [kernel, memory]
---

Wrapper around kmalloc that zero-initializes the allocated memory. Preferred over kmalloc+memset for security (avoids leaking stale kernel data).

## Used In

- Various kernel modules

## See Also

- [[kmalloc]]
- [[kfree]]
