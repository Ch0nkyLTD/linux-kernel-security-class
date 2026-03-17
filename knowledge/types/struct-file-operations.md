---
name: "struct file_operations"
type: type
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/fs.h"
tags: [kernel, chardev]
---

Function pointer table that defines how a file (or character device) responds to open, read, write, ioctl, release, etc. Every chardev module defines one of these and passes it to `cdev_init()`.

## Used In

- All chardev modules
- bouncer.c
- magic_backdoor.c
- prochide.c

## See Also

- [[cdev-init]]
- [[alloc-chrdev-region]]
