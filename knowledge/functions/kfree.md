---
name: "kfree"
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
docs: "https://elixir.bootlin.com/linux/latest/source/mm/slab_common.c"
tags: [kernel, memory]
---

Frees memory previously allocated by kmalloc/kzalloc. Passing NULL is safe (no-op).

## Used In

- All kernel modules with dynamic allocation

## See Also

- [[kmalloc]]
- [[kzalloc]]
