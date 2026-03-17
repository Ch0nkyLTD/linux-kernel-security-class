---
name: "memfd_create"
type: syscall
introduced_in: reflective-payloads
introduced_date: 2026-03-13
scope: used
scope_history:
  - lecture: reflective-payloads
    date: 2026-03-13
  - lecture: shellcode-loaders
    date: 2026-03-17
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man2/memfd_create.2.html"
tags: [syscall, stealth, memory]
---

Creates an anonymous file in memory, returning a file descriptor with no filesystem entry. Combined with `dlopen("/proc/self/fd/N")` or `fexecve()`, enables loading and executing code without touching disk. MFD_CLOEXEC flag auto-closes on exec. Core technique in the reflective payload stealth ladder.

## Used In

- reflective-payloads.md
- shellcode-loaders.md (ex3, ex4)
- capstone Part A (memfd ELF loader)

## See Also

- [[reflective-loading]]
- [[dlopen]]
- [[fexecve]]
