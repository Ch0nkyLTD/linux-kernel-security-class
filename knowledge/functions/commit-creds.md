---
name: "commit_creds"
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
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[struct-cred]]"
  - "[[rcu]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/cred.c"
tags: [kernel, credentials, privesc]
---

Atomically replaces `current->cred` with a new credential struct via `rcu_assign_pointer`. After this call returns and the process returns to userspace, all credential checks see the new values.

## Used In

- privesc.c
- kernel-shellcoding.md
- capstone rootkit

## See Also

- [[prepare-kernel-cred]]
- [[rcu-assign-pointer]]
