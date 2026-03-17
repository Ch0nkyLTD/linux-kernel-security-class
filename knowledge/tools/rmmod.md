---
name: "rmmod"
type: tool
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
prerequisites:
  - "[[insmod]]"
docs: "https://man7.org/linux/man-pages/man8/rmmod.8.html"
tags: [tool, kernel]
---

Removes a loaded kernel module. Calls the function registered with `module_exit()`. If the module hid itself via `list_del_init`, rmmod cannot find it.

## Used In

- ko-basics.md
- linux-kernel-basics.md

## See Also

- [[insmod]]
- [[module-hiding]]
