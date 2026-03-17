---
name: "mmap"
type: syscall
introduced_in: linux-basics
introduced_date: 2026-01-16
scope: mastery
scope_history:
  - lecture: linux-basics
    date: 2026-01-16
  - lecture: linux_file_io
    date: 2026-01-20
  - lecture: reflective-payloads
    date: 2026-03-13
  - lecture: shellcode-loaders
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man2/mmap.2.html"
tags: [syscall, memory]
---

Maps files or anonymous memory into the process address space. Key flags: PROT_READ/WRITE/EXEC, MAP_PRIVATE/SHARED/ANONYMOUS. RWX anonymous mappings (PROT_READ|PROT_WRITE|PROT_EXEC) are used by the MERIDIAN server to create executable shellcode pages -- a major detection indicator.

## Used In

- linux-basics.md
- reflective-payloads.md
- MERIDIAN server
- shellcode-loaders.md

## See Also

- [[mprotect]]
