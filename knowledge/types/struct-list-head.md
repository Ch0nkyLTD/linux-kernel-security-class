---
name: "struct list_head"
type: type
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: promote
    date: 2026-02-10
  - lecture: process-hiding
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/types.h"
tags: [kernel, data-structures]
---

Doubly-linked list node embedded in kernel data structures. Contains `next` and `prev` pointers. The kernel linked list pattern: embed a list_head in your struct, use `container_of()` to get back to the enclosing struct, traverse with `list_for_each_entry()`.

## Used In

- Module list (THIS_MODULE->list)
- task_struct.tasks
- prochide.c hide list

## See Also

- [[container-of]]
- [[list-del-init]]
