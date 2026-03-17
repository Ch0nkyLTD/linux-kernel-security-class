---
name: "read"
type: syscall
introduced_in: linux_file_io
introduced_date: 2026-01-20
scope: introduced
scope_history:
  - lecture: linux_file_io
    date: 2026-01-20
prerequisites:
  - "[[openat]]"
docs: "https://man7.org/linux/man-pages/man2/read.2.html"
tags: [syscall, filesystem]
---

Reads bytes from a file descriptor into a buffer. Fundamental I/O syscall. Returns number of bytes read (0 at EOF, negative on error).

## Used In

- linux_file_io.md
- various modules

## See Also

- [[write-syscall]]
- [[openat]]
