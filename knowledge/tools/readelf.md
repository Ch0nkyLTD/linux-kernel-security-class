---
name: "readelf"
type: tool
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
docs: "https://man7.org/linux/man-pages/man1/readelf.1.html"
tags: [tool, elf]
---

Displays information about ELF files: headers, sections, program headers, dynamic section, symbol tables, relocations. Essential for understanding binary layout. `readelf -d` shows the dynamic section; `readelf -s` shows symbols.

## Used In

- basic_elf.md
- shellcode-loaders.md

## See Also

- [[elf64-ehdr]]
- [[elf64-phdr]]
