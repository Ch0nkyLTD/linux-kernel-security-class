---
name: "strncpy_from_user"
type: function
introduced_in: kprobes
introduced_date: 2026-02-10
scope: introduced
scope_history:
  - lecture: kprobes
    date: 2026-02-10
prerequisites:
  - "[[copy-from-user]]"
docs: "https://elixir.bootlin.com/linux/latest/source/lib/strncpy_from_user.c"
tags: [kernel, memory]
---

Copies a NUL-terminated string from userspace to kernel space, up to a maximum length. Used in kprobe handlers to read filename arguments passed as userspace pointers.

## Used In

- trace_openat.c
- secret.c

## See Also

- [[copy-from-user]]
