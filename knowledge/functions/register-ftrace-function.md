---
name: "register_ftrace_function"
type: function
introduced_in: ftrace
introduced_date: 2026-02-13
scope: used
scope_history:
  - lecture: ftrace
    date: 2026-02-13
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[struct-ftrace-ops]]"
  - "[[ftrace-set-filter-ip]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/trace/ftrace.c"
tags: [kernel, hooking, ftrace]
---

Registers an ftrace callback that fires whenever the target function is called. The callback receives `struct ftrace_regs` giving access to arguments. On AArch64, patches the NOP at function entry to a BL into the ftrace trampoline.

## Used In

- trace_openat_ftrace.c
- bouncer_ftrace.c
- hidemap.c

## See Also

- [[unregister-ftrace-function]]
- [[ftrace-set-filter-ip]]
- [[struct-ftrace-ops]]
