---
name: "cdev_init"
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
  - "[[alloc-chrdev-region]]"
  - "[[struct-file-operations]]"
docs: "https://elixir.bootlin.com/linux/latest/source/fs/char_dev.c"
tags: [kernel, chardev]
---

Initializes a `struct cdev` and associates it with a `file_operations` table. Part of the chardev registration sequence.

## Used In

- bouncer.c
- magic_backdoor.c
- prochide.c

## See Also

- [[cdev-add]]
- [[alloc-chrdev-region]]
