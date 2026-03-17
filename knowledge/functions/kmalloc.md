---
name: "kmalloc"
type: function
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: mastery
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
  - lecture: kprobes
    date: 2026-02-10
prerequisites:
  - "[[gfp-flags]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/slab.h"
tags: [kernel, memory]
---

Allocates physically contiguous kernel memory. Takes a size and GFP flags (GFP_KERNEL for sleepable context, GFP_ATOMIC for atomic context). Every kmalloc must have a matching kfree.

## Used In

- Most kernel modules
- cloak.c (dirent buffer copy)
- prochide.c

## See Also

- [[kfree]]
- [[kzalloc]]
- [[gfp-flags]]
