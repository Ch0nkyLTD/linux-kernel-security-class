---
name: "find_vpid"
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
  - "[[rcu]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/pid.c"
tags: [kernel, process-management]
---

Looks up a `struct pid` by its numeric PID value in the current PID namespace. Must be called under `rcu_read_lock()`. Used with `pid_task()` to get a task_struct from a PID number.

## Used In

- privesc.c
- threadinject.c

## See Also

- [[pid-task]]
- [[rcu-read-lock]]
