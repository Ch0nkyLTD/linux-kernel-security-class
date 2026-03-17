---
name: "list_del_init"
type: function
introduced_in: promote
introduced_date: 2026-02-10
scope: mastery
scope_history:
  - lecture: promote
    date: 2026-02-10
  - lecture: process-hiding
    date: 2026-03-17
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[struct-list-head]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/list.h"
tags: [kernel, linked-lists, stealth]
---

Removes a node from a doubly-linked list and reinitializes its pointers to point to itself. Used for module hiding (`list_del_init(&THIS_MODULE->list)`) to remove the module from the modules list, making it invisible to lsmod and /proc/modules.

## Used In

- ghostmod.c
- capstone rootkit

## See Also

- [[struct-list-head]]
- [[container-of]]
- [[module-hiding]]
