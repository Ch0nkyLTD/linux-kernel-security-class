---
name: "rcu_assign_pointer"
type: function
introduced_in: linux-kernel-basics
introduced_date: 2026-02-06
scope: used
scope_history:
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
prerequisites:
  - "[[rcu]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/rcupdate.h"
tags: [kernel, synchronization, rcu]
---

Publishes a new pointer value with the necessary memory barriers so that RCU readers see either the old or new value, never a torn pointer. Used in commit_creds to update current->cred.

## Used In

- commit_creds (internal)
- promote.md

## See Also

- [[rcu-read-lock]]
- [[rcu]]
