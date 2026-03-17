---
name: "Elf64_Dyn"
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
  - "[[elf64-phdr]]"
docs: "https://man7.org/linux/man-pages/man5/elf.5.html"
tags: [elf, linking]
---

Entry in the ELF dynamic section (PT_DYNAMIC segment). Tags include DT_SYMTAB, DT_STRTAB, DT_NEEDED (shared library dependencies), DT_JMPREL (PLT relocations). Used in reflective loaders to find the symbol table at runtime.

## Used In

- ld_preload_0.md (GOT patching)
- shellcode-loaders.md (find dlopen)

## See Also

- [[elf64-phdr]]
- [[elf64-sym]]
