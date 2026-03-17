---
name: "schedule_timeout_interruptible"
type: function
introduced_in: kthreads
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: kthreads
    date: 2026-03-17
prerequisites:
  - "[[kthread-run]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/time/timer.c"
tags: [kernel, kthreads, scheduling]
---

Sleeps for a specified number of jiffies, but can be woken early by signals (including the implicit wakeup from `kthread_stop()`). Preferred over `ssleep()` for kthreads that need responsive shutdown.

## Used In

- kthread_beacon.ko

## See Also

- [[kthread-should-stop]]
