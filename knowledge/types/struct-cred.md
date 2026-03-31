---
name: "struct cred"
type: type
introduced_in: ko-basics
introduced_date: 2026-01-16
scope: mastery
scope_history:
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
  - lecture: kernel-shellcoding
    date: 2026-03-17
  - lecture: kernel-shellcoding
    date: 2026-03-20
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - "[[task-struct]]"
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/cred.h"
tags: [kernel, credentials]
---

Holds a task's security credentials: four UID/GID pairs (real, effective, saved, filesystem), capability sets, and keyring pointers. Shared via RCU -- never modified in place; always replaced atomically via `commit_creds()`.

## Used In

- privesc.c
- kernel-shellcoding.md
- promote.md

## See Also

- [[prepare-kernel-cred]]
- [[commit-creds]]
- [[task-struct]]
