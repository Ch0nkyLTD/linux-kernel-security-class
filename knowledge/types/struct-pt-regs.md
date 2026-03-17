---
name: "struct pt_regs"
type: type
introduced_in: kprobes
introduced_date: 2026-02-10
scope: mastery
scope_history:
  - lecture: kprobes
    date: 2026-02-10
  - lecture: ftrace
    date: 2026-02-13
  - lecture: syscall-table
    date: 2026-02-17
  - lecture: kernel-code-injection
    date: 2026-02-27
  - lecture: kernel-shellcoding
    date: 2026-03-17
prerequisites:
  - "[[exception-levels]]"
docs: "https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/ptrace.h"
tags: [kernel, aarch64, hooking]
---

Saved CPU register state at a trap/exception boundary. On AArch64, contains `regs[0..30]`, `sp`, `pc`, and `pstate`. Kprobe/ftrace handlers receive this to inspect or modify syscall arguments. The double-pt_regs indirection on AArch64 syscall entry is a key course concept.

## Used In

- All kprobe/ftrace handlers
- trace_openat.c
- bouncer.c
- syscall_hook.c

## See Also

- [[register-kprobe]]
- [[double-pt-regs]]
