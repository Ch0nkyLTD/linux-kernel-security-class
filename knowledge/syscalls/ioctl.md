---
name: "ioctl"
type: syscall
introduced_in: linux-kernel-basics
introduced_date: 2026-02-06
scope: used
scope_history:
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: kernel-shellcoding
    date: 2026-03-17
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - "[[struct-file-operations]]"
docs: "https://man7.org/linux/man-pages/man2/ioctl.2.html"
tags: [syscall, kernel]
---

Device-specific control operations. The chardev's `file_operations.unlocked_ioctl` handler receives the command and argument. In the capstone, used to send shellcode payloads to vulnerable drivers (`VULN_RWX_EXEC`, `VULN_KREAD`, `VULN_KWRITE`).

## Used In

- linux-kernel-basics.md
- vuln_rwx.ko
- vuln_rw.ko
- kernel-shellcoding.md

## See Also

- [[struct-file-operations]]
