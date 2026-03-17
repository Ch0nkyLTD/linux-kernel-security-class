---
name: "prepare_creds"
type: function
introduced_in: linux-kernel-basics
introduced_date: 2026-02-06
scope: used
scope_history:
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: promote
    date: 2026-02-10
prerequisites:
  - "[[struct-cred]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/cred.c"
tags: [kernel, credentials]
---

Allocates a new `struct cred` by copying the calling task's current credentials. Used when you want to modify specific fields (e.g., set euid to 0) rather than replacing all credentials wholesale.

## Used In

- privesc.c
- promote.md

## See Also

- [[prepare-kernel-cred]]
- [[commit-creds]]
