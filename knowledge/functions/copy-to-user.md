---
name: "copy_to_user"
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

Safely copies data from kernel space to userspace. Counterpart to `copy_from_user()`. Returns number of bytes not copied.

## Used In

- chardev modules
- bouncer.c
- magic_backdoor.c

## See Also

- [[copy-from-user]]
