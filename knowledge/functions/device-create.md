---
name: "device_create"
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
  - "[[class-create]]"
docs: "https://elixir.bootlin.com/linux/latest/source/drivers/base/core.c"
tags: [kernel, chardev]
---

Creates a device node in /dev via sysfs/udev. Final step in the chardev registration pattern. After this, userspace can open /dev/<name>.

## Used In

- bouncer.c
- magic_backdoor.c
- prochide.c
- threadinject.c

## See Also

- [[class-create]]
- [[alloc-chrdev-region]]
