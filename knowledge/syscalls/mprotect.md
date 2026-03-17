---
name: "mprotect"
type: syscall
introduced_in: linux-basics
introduced_date: 2026-01-16
scope: used
scope_history:
  - lecture: linux-basics
    date: 2026-01-16
  - lecture: ld_preload_0
    date: 2026-02-03
prerequisites:
  - "[[mmap]]"
docs: "https://man7.org/linux/man-pages/man2/mprotect.2.html"
tags: [syscall, memory]
---

Changes the protection flags on existing memory pages. Used in GOT/PLT patching to make read-only pages writable before overwriting GOT entries. Also used to add PROT_EXEC to data pages for shellcode execution.

## Used In

- ld_preload_0.md (GOT patching)
- linux-basics.md

## See Also

- [[mmap]]
