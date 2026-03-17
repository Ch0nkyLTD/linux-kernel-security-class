---
name: "unregister_ftrace_function"
type: function
introduced_in: ftrace
introduced_date: 2026-02-13
scope: introduced
scope_history:
  - lecture: ftrace
    date: 2026-02-13
prerequisites:
  - "[[register-ftrace-function]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/trace/ftrace.c"
tags: [kernel, hooking, ftrace]
---

Unregisters an ftrace callback and restores the NOP at the function entry point.

## Used In

- trace_openat_ftrace.c
- bouncer_ftrace.c
- hidemap.c

## See Also

- [[register-ftrace-function]]
