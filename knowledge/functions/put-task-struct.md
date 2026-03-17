---
name: "put_task_struct"
type: function
introduced_in: linux-kernel-basics
introduced_date: 2026-02-06
scope: introduced
scope_history:
  - lecture: linux-kernel-basics
    date: 2026-02-06
prerequisites:
  - "[[get-task-struct]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/sched/task.h"
tags: [kernel, process-management]
---

Decrements the reference count on a task_struct. When the count reaches zero, the task_struct is freed.

## Used In

- threadinject.c
- privesc.c

## See Also

- [[get-task-struct]]
