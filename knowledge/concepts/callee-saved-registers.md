---
name: "callee-saved registers"
type: concept
introduced_in: armv8-a_crash_course
introduced_date: 2026-01-13
scope: used
scope_history:
  - lecture: armv8-a_crash_course
    date: 2026-01-13
  - lecture: kernel-shellcoding
    date: 2026-03-17
prerequisites:
  - (none)
docs: ""
tags: [aarch64, asm]
---

AArch64 calling convention: registers x19-x28 and x29(FP) are preserved across function calls. x0-x18 and x30(LR) may be clobbered. Kernel shellcode stores function addresses in x19/x20 so they survive `blr` calls. `stp`/`ldp` save and restore these on the stack.

## Used In

- kernel-shellcoding.md
- armv8-a_crash_course.md

## See Also

- [[movz-movk-encoding]]
