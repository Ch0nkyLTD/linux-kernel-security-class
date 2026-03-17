---
name: "kthread_run"
type: function
introduced_in: kthreads
introduced_date: 2026-03-17
scope: used
scope_history:
  - lecture: kthreads
    date: 2026-03-17
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[task-struct]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/kthread.h"
tags: [kernel, kthreads]
---

Macro that creates and immediately starts a kernel thread. Expands to `kthread_create()` + `wake_up_process()`. Returns a `task_struct *` (or ERR_PTR on failure). The thread runs until its function returns or `kthread_stop()` is called.

## Used In

- kthread_beacon.ko (inline example)
- capstone rootkit (network backdoor)

## See Also

- [[kthread-stop]]
- [[kthread-should-stop]]
