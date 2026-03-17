---
name: "struct linux_dirent64"
type: type
introduced_in: linux_filesystem_ops
introduced_date: 2026-01-30
scope: used
scope_history:
  - lecture: linux_filesystem_ops
    date: 2026-01-30
  - lecture: kprobes
    date: 2026-02-10
  - lecture: process-hiding
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man2/getdents64.2.html"
tags: [kernel, filesystem, stealth]
---

Directory entry structure returned by the `getdents64` syscall. Fields: `d_ino`, `d_off`, `d_reclen` (entry size), `d_type`, `d_name[]`. The rootkit hides entries by adjusting `d_reclen` of the previous entry to skip over the hidden one.

## Used In

- cloak.c
- prochide.c
- capstone rootkit (file and process hiding)

## See Also

- [[getdents64]]
- [[file-hiding]]
