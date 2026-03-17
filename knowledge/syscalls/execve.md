---
name: "execve"
type: syscall
introduced_in: linux-basics
introduced_date: 2026-01-16
scope: mastery
scope_history:
  - lecture: linux-basics
    date: 2026-01-16
  - lecture: elf-in-depth-and-process-creation
    date: 2026-01-30
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: kernel-shellcoding
    date: 2026-03-17
prerequisites:
  - "[[fork]]"
docs: "https://man7.org/linux/man-pages/man2/execve.2.html"
tags: [syscall, process-management]
---

Replaces the current process image with a new program. The kernel parses the ELF headers, maps PT_LOAD segments, sets up the stack, and jumps to the entry point. In the capstone chain, used to run `insmod` after privilege escalation, or to spawn `/bin/sh` for a reverse shell.

## Used In

- elf-in-depth-and-process-creation.md
- capstone chain (Stage 2->3)

## See Also

- [[fork]]
