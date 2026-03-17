---
name: "file hiding"
type: concept
introduced_in: kprobes
introduced_date: 2026-02-10
scope: used
scope_history:
  - lecture: kprobes
    date: 2026-02-10
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[register-kretprobe]]"
  - "[[struct-linux-dirent64]]"
docs: ""
tags: [kernel, stealth, rootkit]
---

Technique using a kretprobe on `getdents64` to filter directory entries before they reach userspace. The return handler copies the dirent buffer to kernel space, walks the linked entries, removes those matching a hidden prefix by adjusting `d_reclen`, and copies the modified buffer back. `ls` sees a clean directory.

## Used In

- cloak.c
- capstone rootkit (Part B)

## See Also

- [[process-hiding-concept]]
- [[module-hiding]]
- [[struct-linux-dirent64]]
