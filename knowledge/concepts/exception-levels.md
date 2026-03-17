---
name: "exception levels"
type: concept
introduced_in: armv8-a_crash_course
introduced_date: 2026-01-13
scope: mastery
scope_history:
  - lecture: armv8-a_crash_course
    date: 2026-01-13
  - lecture: ko-basics
    date: 2026-01-16
  - lecture: linux-kernel-basics
    date: 2026-02-06
  - lecture: virtual-memory-deepdive
    date: 2026-02-20
  - lecture: kernel-shellcoding
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://developer.arm.com/documentation/den0024/latest/"
tags: [aarch64, architecture]
---

AArch64 privilege hierarchy: EL0 (userspace), EL1 (kernel), EL2 (hypervisor), EL3 (secure monitor). Transitions occur via `svc` (EL0->EL1), `hvc` (EL1->EL2), or exceptions. The syscall path and all kernel hooking operates at the EL0/EL1 boundary.

## Used In

- armv8-a_crash_course.md
- ko-basics.md
- kernel-shellcoding.md

## See Also

- [[struct-pt-regs]]
