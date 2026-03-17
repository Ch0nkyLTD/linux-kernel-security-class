---
name: "lsmod"
type: tool
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: introduced
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man8/lsmod.8.html"
tags: [tool, kernel]
---

Lists loaded kernel modules by reading /proc/modules. A module that has called `list_del_init(&THIS_MODULE->list)` will not appear in this output.

## Used In

- ko-basics.md
- ghostmod.c (demonstrates hiding from lsmod)

## See Also

- [[insmod]]
- [[module-hiding]]
