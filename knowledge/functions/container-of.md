---
name: "container_of"
type: function
introduced_in: promote
introduced_date: 2026-02-10
scope: used
scope_history:
  - lecture: promote
    date: 2026-02-10
  - lecture: kprobes
    date: 2026-02-10
prerequisites:
  - "[[struct-list-head]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/container_of.h"
tags: [kernel, linked-lists]
---

Macro that computes the address of the enclosing struct given a pointer to one of its members. Central to the Linux kernel linked list pattern where `list_head` is embedded in larger structs.

## Used In

- All list traversal code
- promote.md

## See Also

- [[struct-list-head]]
