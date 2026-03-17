---
name: "dl_iterate_phdr"
type: function
introduced_in: ld_preload_0
introduced_date: 2026-02-03
scope: introduced
scope_history:
  - lecture: ld_preload_0
    date: 2026-02-03
prerequisites:
  - "[[elf64-phdr]]"
docs: "https://man7.org/linux/man-pages/man3/dl_iterate_phdr.3.html"
tags: [userland, elf, linking]
---

Iterates over all loaded shared objects, providing access to each object's program headers. Used for GOT/PLT patching to find a library's dynamic section and relocation entries.

## Used In

- ld_preload_0.md (GOT patching technique)

## See Also

- [[elf64-phdr]]
- [[got-plt]]
