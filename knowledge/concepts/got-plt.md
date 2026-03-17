---
name: "GOT/PLT"
type: concept
introduced_in: compiler-linking
introduced_date: 2026-01-20
scope: used
scope_history:
  - lecture: compiler-linking
    date: 2026-01-20
  - lecture: ld_preload_0
    date: 2026-02-03
prerequisites:
  - "[[dynamic-linking]]"
docs: ""
tags: [userland, elf, linking]
---

Global Offset Table and Procedure Linkage Table -- the two-table mechanism for dynamic symbol resolution. The PLT contains stub code that jumps through the GOT. On first call, the GOT entry points to the resolver; after resolution, it points directly to the target function. GOT patching replaces resolved entries to redirect calls.

## Used In

- ld_preload_0.md (GOT/PLT patching exercises)
- HW3

## See Also

- [[dynamic-linking]]
- [[elf64-rela]]
