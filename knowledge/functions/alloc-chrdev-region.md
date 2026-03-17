---
name: "alloc_chrdev_region"
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
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/fs/char_dev.c"
tags: [kernel, chardev]
---

Dynamically allocates a range of character device numbers (major/minor). First step in the chardev registration pattern. Must be followed by cdev_init, cdev_add, class_create, device_create.

## Used In

- bouncer.c
- magic_backdoor.c
- prochide.c
- threadinject.c

## See Also

- [[cdev-init]]
- [[cdev-add]]
- [[class-create]]
- [[device-create]]
