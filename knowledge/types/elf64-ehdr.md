---
name: "Elf64_Ehdr"
type: type
introduced_in: basic_elf
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: basic_elf
    date: 2026-01-16
  - lecture: elf-in-depth-and-process-creation
    date: 2026-01-30
  - lecture: shellcode-loaders
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man5/elf.5.html"
tags: [elf, userland]
---

ELF file header. Contains magic bytes, machine type, entry point (`e_entry`), program header offset (`e_phoff`), section header offset (`e_shoff`), and their counts. First structure read when loading any ELF binary or shared object.

## Used In

- elf-in-depth-and-process-creation.md
- shellcode-loaders.md (ELF walk)
- exploit_kload_hard_solution.c

## See Also

- [[elf64-phdr]]
