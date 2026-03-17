---
name: "call_usermodehelper"
type: function
introduced_in: kernel-code-injection
introduced_date: 2026-02-27
scope: used
scope_history:
  - lecture: kernel-code-injection
    date: 2026-02-27
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: full-chain-rootkit
    date: 2026-03-17
prerequisites:
  - "[[kthread-run]]"
docs: "https://elixir.bootlin.com/linux/latest/source/kernel/umh.c"
tags: [kernel, injection]
---

Spawns a userspace process from kernel context. The kernel uses this internally for modprobe, hotplug, and coredump helpers. In the capstone, used as an alternative to `execve` for loading the rootkit from kernel shellcode.

## Used In

- modprobe_path exploit
- kernel-code-injection.md

## See Also

- [[modprobe-path]]
