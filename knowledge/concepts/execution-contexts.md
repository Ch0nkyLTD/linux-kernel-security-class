---
name: "execution contexts"
type: concept
introduced_in: promote
introduced_date: 2026-02-10
scope: used
scope_history:
  - lecture: promote
    date: 2026-02-10
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - "[[gfp-flags]]"
docs: ""
tags: [kernel, synchronization]
---

Kernel code runs in either process context (can sleep, has a `current` task, uses GFP_KERNEL) or interrupt/atomic context (cannot sleep, uses GFP_ATOMIC, spin_lock_irqsave). Kprobe handlers run in atomic context. Kthreads run in process context. Choosing the wrong allocation or locking primitive for the context causes deadlocks or BUGs.

## Used In

- promote.md (contexts lecture)
- kthreads.md

## See Also

- [[gfp-flags]]
- [[kthread-run]]
