---
name: "/proc/kallsyms"
type: tool
introduced_in: kallsyms
introduced_date: 2026-02-17
scope: used
scope_history:
  - lecture: kallsyms
    date: 2026-02-17
  - lecture: syscall-table
    date: 2026-02-17
  - lecture: kernel-shellcoding
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://www.kernel.org/doc/html/latest/admin-guide/sysfs-rules.html"
tags: [tool, kernel, symbols]
---

Pseudo-file listing all kernel symbol addresses. Format: `<address> <type> <name> [module]`. Readable when `kptr_restrict=0`. Used by exploits to resolve function addresses (prepare_kernel_cred, commit_creds, sys_call_table) at runtime without hardcoding offsets.

## Used In

- kallsyms.md
- kernel-shellcoding.md
- all capstone exploit solutions

## See Also

- [[kallsyms-lookup-name]]
- [[kprobe-bootstrap-trick]]
