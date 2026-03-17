---
name: "kthread_should_stop"
type: function
introduced_in: kthreads
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - "[[kthread-run]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/kthread.h"
tags: [kernel, kthreads]
---

Returns true when `kthread_stop()` has been called. The thread function must check this regularly in its loop to allow clean shutdown.

## Used In

- kthread_beacon.ko
- capstone rootkit

## See Also

- [[kthread-stop]]
- [[kthread-run]]
