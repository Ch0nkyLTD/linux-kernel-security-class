---
name: "rcu_read_lock"
type: function
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
  - "[[rcu]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/rcupdate.h"
tags: [kernel, synchronization, rcu]
---

Marks the beginning of an RCU read-side critical section. Disables preemption to ensure the reader sees a consistent snapshot. Must be paired with `rcu_read_unlock()`. No sleeping allowed between lock/unlock.

## Used In

- magic_backdoor.c
- prochide.c
- privesc.c

## See Also

- [[rcu-assign-pointer]]
- [[rcu]]
