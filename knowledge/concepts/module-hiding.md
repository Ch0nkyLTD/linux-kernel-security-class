---
name: "module hiding"
type: concept
introduced_in: promote
introduced_date: 2026-02-10
scope: used
scope_history:
  - lecture: promote
    date: 2026-02-10
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[struct-list-head]]"
  - "[[list-del-init]]"
docs: ""
tags: [kernel, stealth, rootkit]
---

Removing a kernel module from the modules linked list via `list_del_init(&THIS_MODULE->list)`. After this, `lsmod` and `/proc/modules` no longer show the module. The module's code and data remain in memory. Does NOT hide `/sys/module/<name>/` (requires separate kobject manipulation).

## Used In

- ghostmod.c
- capstone rootkit

## See Also

- [[list-del-init]]
- [[file-hiding]]
