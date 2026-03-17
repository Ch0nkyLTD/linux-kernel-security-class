---
name: "double pt_regs indirection"
type: concept
introduced_in: kprobes
introduced_date: 2026-02-10
scope: used
scope_history:
  - lecture: kprobes
    date: 2026-02-10
  - lecture: syscall-table
    date: 2026-02-17
prerequisites:
  - "[[struct-pt-regs]]"
  - "[[register-kprobe]]"
docs: ""
tags: [kernel, hooking, aarch64]
---

On AArch64 syscall entry, the kprobe handler's `pt_regs` are the handler's own saved registers, not the user's. The user's `pt_regs` (containing syscall arguments) are at `(struct pt_regs *)regs->regs[0]`. This extra dereference is required to access the actual syscall arguments in kprobe handlers on AArch64.

## Used In

- trace_openat.c
- bouncer.c
- cloak.c
- prochide.c

## See Also

- [[struct-pt-regs]]
- [[register-kprobe]]
