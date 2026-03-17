---
name: "class_create"
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
  - "[[cdev-add]]"
docs: "https://elixir.bootlin.com/linux/latest/source/drivers/base/class.c"
tags: [kernel, chardev]
---

Creates a device class in sysfs. Needed so that udev/mdev can automatically create the /dev node when device_create is called.

## Used In

- bouncer.c
- magic_backdoor.c
- prochide.c

## See Also

- [[device-create]]
- [[cdev-add]]
