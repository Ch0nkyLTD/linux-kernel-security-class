---
name: "pahole"
type: tool
introduced_in: kernel-shellcoding
introduced_date: 2026-03-20
scope: used
scope_history:
  - lecture: kernel-shellcoding
    date: 2026-03-20
prerequisites:
  - "[[task-struct]]"
  - "[[struct-cred]]"
docs: "https://manpages.debian.org/pahole"
tags: [tool, debugging, structs]
---

Reads DWARF debug info from vmlinux and prints struct layouts with byte offsets. Used to extract hardcoded offsets for shellcode that directly manipulates kernel structs (e.g., task_struct->cred->uid).

## Used In

- kernel-shellcoding.md (offset discovery for raw cred-zeroing)
- inclass/kernel_shellcode/find_offsets.sh

## See Also

- [[task-struct]]
- [[struct-cred]]
