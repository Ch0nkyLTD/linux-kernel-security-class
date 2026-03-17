---
name: "process hiding"
type: concept
introduced_in: process-hiding
introduced_date: 2026-03-17
scope: used
scope_history:
  - lecture: process-hiding
    date: 2026-03-17
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[file-hiding]]"
docs: ""
tags: [kernel, stealth, rootkit]
---

Same getdents64 kretprobe technique as file hiding, but applied to `/proc` directory entries. Filters numeric PID entries to hide specific processes from `ps`. Can also use a magic GID bypass (`in_group_p()`) so hidden processes can see each other.

## Used In

- prochide.c
- capstone rootkit (Part B)

## See Also

- [[file-hiding]]
- [[module-hiding]]
