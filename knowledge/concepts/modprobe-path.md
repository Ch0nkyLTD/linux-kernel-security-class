---
name: "modprobe_path"
type: concept
introduced_in: capstone-kernel
introduced_date: 2026-02-24
scope: used
scope_history:
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[call-usermodehelper]]"
docs: ""
tags: [kernel, privesc]
---

Kernel variable (`/sbin/modprobe` by default) specifying the path to the module loader. When the kernel encounters an unrecognized binary format, it calls `call_usermodehelper(modprobe_path, ...)` as root. Overwriting this path with a malicious script gives root code execution without ever modifying the calling process's credentials.

## Used In

- exploit_modprobe_solution.c (Challenge 9)
- full-chain-rootkit.md

## See Also

- [[call-usermodehelper]]
- [[prepare-kernel-cred]]
