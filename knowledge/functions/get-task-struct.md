---
name: "get_task_struct"
type: function
introduced_in: linux-kernel-basics
introduced_date: 2026-02-06
scope: introduced
scope_history:
  - lecture: linux-kernel-basics
    date: 2026-02-06
prerequisites:
  - "[[task-struct]]"
  - "[[rcu]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/sched/task.h"
tags: [kernel, process-management]
---

Increments the reference count on a task_struct, preventing it from being freed. Must be paired with `put_task_struct()`. Needed when you want to hold a task reference outside of RCU protection.

## Used In

- threadinject.c
- privesc.c

## See Also

- [[put-task-struct]]
- [[pid-task]]
