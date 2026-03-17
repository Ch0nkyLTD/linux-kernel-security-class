---
name: "reflective loading"
type: concept
introduced_in: reflective-payloads
introduced_date: 2026-03-13
scope: used
scope_history:
  - lecture: reflective-payloads
    date: 2026-03-13
  - lecture: shellcode-loaders
    date: 2026-03-17
prerequisites:
  - "[[dynamic-linking]]"
  - "[[memfd-create]]"
docs: ""
tags: [userland, stealth, injection]
---

Loading and executing code entirely from memory without touching the filesystem. The stealth ladder: /tmp+dlopen -> /dev/shm+dlopen -> memfd+dlopen -> no-libc reflective loader. The most advanced form parses ELF headers, resolves symbols, and applies relocations manually, never calling the standard dynamic linker.

## Used In

- reflective-payloads.md
- shellcode-loaders.md
- exploit_kload_hard_solution.c (kernel reflective)

## See Also

- [[memfd-create]]
- [[dlopen]]
- [[elf64-ehdr]]
