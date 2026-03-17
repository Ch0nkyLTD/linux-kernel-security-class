---
name: "movz/movk encoding"
type: concept
introduced_in: kernel-shellcoding
introduced_date: 2026-03-17
scope: used
scope_history:
  - lecture: kernel-shellcoding
    date: 2026-03-17
  - lecture: armv8-a_crash_course
    date: 2026-01-13
prerequisites:
  - "[[callee-saved-registers]]"
docs: ""
tags: [aarch64, asm, shellcoding]
---

AArch64 technique for loading 64-bit immediates: `movz` zeros the register and sets bits [15:0], then three `movk` (move-keep) instructions set bits [31:16], [47:32], [63:48] without disturbing the other lanes. Four instructions to load any 64-bit address. Core of the kernel privesc shellcode.

## Used In

- kernel-shellcoding.md
- exploit_rwx_solution.c

## See Also

- [[callee-saved-registers]]
