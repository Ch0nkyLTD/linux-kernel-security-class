---
name: "Elf64_Sym"
type: type
introduced_in: ld_preload_0
introduced_date: 2026-02-03
scope: used
scope_history:
  - lecture: ld_preload_0
    date: 2026-02-03
  - lecture: shellcode-loaders
    date: 2026-03-17
prerequisites:
  - "[[elf64-dyn]]"
docs: "https://man7.org/linux/man-pages/man5/elf.5.html"
tags: [elf, linking]
---

ELF symbol table entry. Fields: `st_name` (offset into string table), `st_value` (symbol address/offset), `st_size`, `st_info` (type and binding). Used in reflective loaders to find exported functions like `dlopen` by walking the symbol table.

## Used In

- shellcode-loaders.md (ELF walk)
- ld_preload_0.md

## See Also

- [[elf64-dyn]]
- [[elf64-rela]]
