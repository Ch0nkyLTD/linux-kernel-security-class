---
name: "alloc_page"
type: function
introduced_in: kernel-code-injection
introduced_date: 2026-02-27
scope: introduced
scope_history:
  - lecture: kernel-code-injection
    date: 2026-02-27
prerequisites:
  - "[[page-tables]]"
  - "[[gfp-flags]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/gfp.h"
tags: [kernel, memory]
---

Allocates a single physical page. Returns a `struct page *`. Used in PTE construction to get a physical page that will be mapped into a target process's address space.

## Used In

- threadinject.c (PTE construction path)

## See Also

- [[set-pte]]
