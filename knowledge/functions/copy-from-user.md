---
name: "copy_from_user"
type: function
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
  - "[[exception-levels]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/uaccess.h"
tags: [kernel, memory]
---

Safely copies data from userspace to kernel space with access checks. Returns the number of bytes that could NOT be copied (0 on success). Required because direct userspace pointer dereference from kernel context triggers a fault on architectures with PAN enabled.

## Used In

- chardev modules
- bouncer.c
- magic_backdoor.c
- prochide.c

## See Also

- [[copy-to-user]]
