---
name: "dmesg"
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
  - (none)
docs: "https://man7.org/linux/man-pages/man1/dmesg.1.html"
tags: [tool, kernel, debugging]
---

Reads the kernel ring buffer (printk/pr_info output). Primary debugging tool for kernel modules. Also a detection surface: rootkit loading and driver exploits often leave messages in dmesg.

## Used In

- ko-basics.md
- all kernel module development

## See Also

- [[insmod]]
