---
name: "LD_PRELOAD"
type: concept
introduced_in: ld_preload_0
introduced_date: 2026-02-03
scope: used
scope_history:
  - lecture: ld_preload_0
    date: 2026-02-03
  - lecture: capstone-kernel
    date: 2026-02-24
  - lecture: shellcode-loaders
    date: 2026-03-17
prerequisites:
  - "[[dynamic-linking]]"
docs: "https://man7.org/linux/man-pages/man8/ld.so.8.html"
tags: [userland, hooking, stealth]
---

Environment variable that instructs the dynamic linker to load a specified shared object before all others. Functions defined in the preloaded library shadow those in later libraries (e.g., libc), enabling transparent interception. Core mechanism for userland rootkits.

## Used In

- ld_preload_0.md
- HW3
- capstone Part A
- shellcode-loaders.md (ex3)

## See Also

- [[dlsym]]
- [[dynamic-linking]]
