---
name: "GFP flags"
type: concept
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/gfp_types.h"
tags: [kernel, memory]
---

Get Free Pages flags controlling kernel memory allocation behavior. GFP_KERNEL: may sleep, for process context. GFP_ATOMIC: never sleeps, for interrupt/atomic context. Using GFP_KERNEL in atomic context causes a BUG; using GFP_ATOMIC in process context wastes emergency reserves.

## Used In

- All kernel modules with dynamic allocation

## See Also

- [[kmalloc]]
- [[execution-contexts]]
