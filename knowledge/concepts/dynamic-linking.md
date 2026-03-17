---
name: "dynamic linking"
type: concept
introduced_in: compiler-linking
introduced_date: 2026-01-20
scope: mastery
scope_history:
  - lecture: compiler-linking
    date: 2026-01-20
  - lecture: ld_preload_0
    date: 2026-02-03
  - lecture: reflective-payloads
    date: 2026-03-13
  - lecture: shellcode-loaders
    date: 2026-03-17
prerequisites:
  - (none)
docs: ""
tags: [userland, elf, linking]
---

Runtime resolution of shared library symbols via the dynamic linker (ld-linux). Key mechanisms: PLT (Procedure Linkage Table) for lazy binding, GOT (Global Offset Table) for resolved addresses. LD_PRELOAD exploits the symbol search order to interpose functions.

## Used In

- compiler-linking.md
- ld_preload_0.md
- capstone Part A

## See Also

- [[got-plt]]
- [[ld-preload]]
- [[dlopen]]
