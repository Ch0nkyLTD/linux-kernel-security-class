---
name: "pid_task"
type: function
introduced_in: linux-kernel-basics
introduced_date: 2026-02-06
scope: used
scope_history:
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: kernel-code-injection
    date: 2026-02-27
prerequisites:
  - "[[find-vpid]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/pid.c"
tags: [kernel, process-management]
---

Converts a `struct pid` to its `task_struct` for a given PID type. Returns NULL if the task no longer exists. Must be called under RCU protection.

## Used In

- privesc.c
- threadinject.c

## See Also

- [[find-vpid]]
- [[get-task-struct]]
