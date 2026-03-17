---
name: "ftrace_set_filter_ip"
type: function
introduced_in: ftrace
introduced_date: 2026-02-13
scope: introduced
scope_history:
  - lecture: ftrace
    date: 2026-02-13
prerequisites:
  - "[[struct-ftrace-ops]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/trace/ftrace.c"
tags: [kernel, hooking, ftrace]
---

Restricts an ftrace_ops to fire only for a specific function address. Must be called before `register_ftrace_function()`. Without this filter, the callback would fire for every traced function in the kernel.

## Used In

- trace_openat_ftrace.c
- bouncer_ftrace.c
- hidemap.c

## See Also

- [[register-ftrace-function]]
- [[struct-ftrace-ops]]
