---
name: "struct ftrace_ops"
type: type
introduced_in: ftrace
introduced_date: 2026-02-13
scope: introduced
scope_history:
  - lecture: ftrace
    date: 2026-02-13
prerequisites:
  - (none)
docs: "https://elixir.bootlin.com/linux/latest/source/include/linux/ftrace.h"
tags: [kernel, hooking, ftrace]
---

Describes an ftrace callback registration. Key fields: `.func` (the callback), `.flags` (e.g., FTRACE_OPS_FL_IPMODIFY for blocking hooks, FL_RECURSION for recursion protection). Passed to `register_ftrace_function()`.

## Used In

- trace_openat_ftrace.c
- bouncer_ftrace.c
- hidemap.c

## See Also

- [[register-ftrace-function]]
- [[ftrace-set-filter-ip]]
