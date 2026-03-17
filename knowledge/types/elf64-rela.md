---
name: "Elf64_Rela"
type: type
introduced_in: ld_preload_0
introduced_date: 2026-02-03
scope: used
scope_history:
  - lecture: ld_preload_0
    date: 2026-02-03
  - lecture: reflective-payloads
    date: 2026-03-13
prerequisites:
  - "[[elf64-sym]]"
docs: "https://man7.org/linux/man-pages/man5/elf.5.html"
tags: [elf, linking]
---

ELF relocation entry with addend. Fields: `r_offset` (where to apply), `r_info` (symbol index + relocation type), `r_addend`. AArch64 relocation types include R_AARCH64_JUMP_SLOT (PLT), R_AARCH64_RELATIVE, R_AARCH64_GLOB_DAT, R_AARCH64_CALL26 (BL).

## Used In

- ld_preload_0.md (GOT patching)
- exploit_kload_hard_solution.c (11 reloc types)

## See Also

- [[elf64-sym]]
- [[got-plt]]
