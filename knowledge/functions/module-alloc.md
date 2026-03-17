---
name: "module_alloc"
type: function
introduced_in: kernel-code-injection
introduced_date: 2026-02-27
scope: used
scope_history:
  - lecture: kernel-code-injection
    date: 2026-02-27
  - lecture: kernel-shellcoding
    date: 2026-03-17
prerequisites:
  - "[[virtual-memory]]"
docs: "https://elixir.bootlin.com/linux/latest/source/arch/arm64/kernel/module.c"
tags: [kernel, memory, injection]
---

Allocates executable memory in the kernel module region (within BL range of the kernel text). Used by the module loader and by vuln_rwx.ko to create RWX pages for JIT execution. In Challenge 10b, called from shellcode to allocate space for a reflectively loaded module.

## Used In

- vuln_rwx.ko
- exploit_kload_hard_solution.c

## See Also

- [[set-pte]]
