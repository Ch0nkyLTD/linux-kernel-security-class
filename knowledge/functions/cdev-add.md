---
name: "cdev_add"
type: function
introduced_in: linux-kernel-basics
introduced_date: 2026-02-06
scope: used
scope_history:
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
prerequisites:
  - "[[cdev-init]]"
docs: "https://elixir.bootlin.com/linux/latest/source/fs/char_dev.c"
tags: [kernel, chardev]
---

Adds a character device to the system, making it live. After this call, the device can receive open/read/write/ioctl calls.

## Used In

- bouncer.c
- magic_backdoor.c
- prochide.c

## See Also

- [[cdev-init]]
- [[alloc-chrdev-region]]
