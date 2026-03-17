---
name: "KASLR"
type: concept
introduced_in: kernel-shellcoding
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: kernel-shellcoding
    date: 2026-03-17
prerequisites:
  - "[[virtual-memory]]"
docs: ""
tags: [kernel, security, mitigations]
---

Kernel Address Space Layout Randomization -- randomizes the kernel text base address on each boot. Disabled in the lab (`nokaslr`) for simplicity. With KASLR enabled, an attacker needs an info leak to determine the kernel slide before using hardcoded addresses.

## Used In

- kernel-shellcoding.md

## See Also

- [[proc-kallsyms]]
- [[virtual-memory]]
