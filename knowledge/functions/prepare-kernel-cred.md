---
name: "prepare_kernel_cred"
type: function
introduced_in: linux-kernel-basics
introduced_date: 2026-02-06
scope: mastery
scope_history:
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
  - lecture: kprobes
    date: 2026-02-10
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: kernel-shellcoding
    date: 2026-03-17
  - lecture: kernel-shellcoding
    date: 2026-03-20
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[task-struct]]"
  - "[[struct-cred]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/cred.c"
tags: [kernel, credentials, privesc]
---

Allocates a new `struct cred` with all UIDs/GIDs set to 0 (root) when called with NULL. Core primitive for kernel privilege escalation. Used in combination with `commit_creds()` to elevate a process to root from kernel context.

## Used In

- privesc.c
- kernel-shellcoding.md (Challenge 7 shellcode)
- capstone rootkit

## See Also

- [[commit-creds]]
- [[struct-cred]]
- [[current-macro]]
