---
name: "task_struct"
type: type
introduced_in: linux-basics
introduced_date: 2026-01-16
scope: mastery
scope_history:
  - lecture: linux-basics
    date: 2026-01-16
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
  - lecture: kprobes
    date: 2026-02-10
  - lecture: kernel-code-injection
    date: 2026-02-27
  - lecture: process-hiding
    date: 2026-03-17
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/sched.h"
tags: [kernel, process-management]
---

The central kernel data structure representing a process or thread. Contains pointers to credentials (`cred`), memory descriptor (`mm`), file table, PID, scheduling state, and the `tasks` list_head for the process list. Accessed via the `current` macro for the running task.

## Used In

- Nearly all kernel modules
- privesc.c
- prochide.c
- threadinject.c

## See Also

- [[struct-cred]]
- [[mm-struct]]
- [[current-macro]]
