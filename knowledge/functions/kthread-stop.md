---
name: "kthread_stop"
type: function
introduced_in: kthreads
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - "[[kthread-run]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/kthread.c"
tags: [kernel, kthreads]
---

Signals a kthread to stop, wakes it from any interruptible sleep, and blocks until it exits. Sets the flag that `kthread_should_stop()` checks. Must be called in module_exit to avoid leaked threads.

## Used In

- kthread_beacon.ko
- capstone rootkit

## See Also

- [[kthread-run]]
- [[kthread-should-stop]]
