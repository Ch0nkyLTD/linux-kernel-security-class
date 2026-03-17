---
name: "regs_return_value"
type: function
introduced_in: kprobes
introduced_date: 2026-02-10
scope: introduced
scope_history:
  - lecture: kprobes
    date: 2026-02-10
prerequisites:
  - "[[struct-pt-regs]]"
docs: "https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/ptrace.h"
tags: [kernel, kprobes]
---

Extracts the return value from a `pt_regs` struct. On AArch64, reads `regs->regs[0]` (x0). Used in kretprobe return handlers to inspect or modify syscall return values.

## Used In

- cloak.c
- prochide.c

## See Also

- [[struct-pt-regs]]
- [[register-kretprobe]]
