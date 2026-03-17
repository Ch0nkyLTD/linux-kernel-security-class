---
name: "write"
type: syscall
introduced_in: linux_file_io
introduced_date: 2026-01-20
scope: used
scope_history:
  - lecture: linux_file_io
    date: 2026-01-20
  - lecture: aarch64_asm_snippets
    date: 2026-01-13
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man2/write.2.html"
tags: [syscall, filesystem]
---

Writes bytes from a buffer to a file descriptor. In AArch64 assembly: load fd in x0, buffer in x1, length in x2, syscall number in x8, then `svc #0`.

## Used In

- aarch64_asm_snippets.md (hello world)
- linux_file_io.md

## See Also

- [[read]]
