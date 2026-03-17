---
name: "Elf64_Phdr"
type: type
introduced_in: elf-in-depth-and-process-creation
introduced_date: 2026-01-30
scope: used
scope_history:
  - lecture: elf-in-depth-and-process-creation
    date: 2026-01-30
  - lecture: shellcode-loaders
    date: 2026-03-17
prerequisites:
  - "[[elf64-ehdr]]"
docs: "https://man7.org/linux/man-pages/man5/elf.5.html"
tags: [elf, userland]
---

ELF program header describing a segment to be loaded. Key fields: `p_type` (PT_LOAD, PT_DYNAMIC, PT_INTERP), `p_vaddr`, `p_memsz`, `p_flags`. The kernel's ELF loader maps PT_LOAD segments into memory.

## Used In

- elf-in-depth-and-process-creation.md
- shellcode-loaders.md

## See Also

- [[elf64-ehdr]]
