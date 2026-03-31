---
name: "direct cred zeroing"
type: concept
introduced_in: kernel-shellcoding
introduced_date: 2026-03-20
scope: used
scope_history:
  - lecture: kernel-shellcoding
    date: 2026-03-20
prerequisites:
  - "[[task-struct]]"
  - "[[struct-cred]]"
  - "[[current-macro]]"
docs: ""
tags: [kernel, credentials, shellcode, exploit]
---

Privilege escalation technique that bypasses the prepare_kernel_cred/commit_creds API by directly zeroing uid/gid fields in the live cred struct. Uses mrs sp_el0 to find current task_struct, chases the cred pointer, and writes zeros with str xzr. 7 instructions / 28 bytes on AArch64. Faster and smaller than the API approach but requires hardcoded struct offsets and bypasses RCU safety.

## Used In

- kernel-shellcoding.md (Act II)
- inclass/kernel_shellcode/raw_privesc.c

## See Also

- [[prepare-kernel-cred]]
- [[commit-creds]]
- [[movz-movk-encoding]]
