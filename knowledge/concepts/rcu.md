---
name: "RCU"
type: concept
introduced_in: linux-kernel-basics
introduced_date: 2026-02-06
scope: used
scope_history:
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
  - lecture: process-hiding
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://www.kernel.org/doc/html/latest/RCU/"
tags: [kernel, synchronization]
---

Read-Copy-Update -- a synchronization mechanism optimized for read-heavy workloads. Readers (rcu_read_lock/unlock) never block. Writers make a copy, modify it, and publish via rcu_assign_pointer; old copies are freed after a grace period. Used throughout the kernel for credential updates, task list traversal, and more.

## Used In

- commit_creds (internal)
- prochide.c
- magic_backdoor.c

## See Also

- [[rcu-read-lock]]
- [[rcu-assign-pointer]]
