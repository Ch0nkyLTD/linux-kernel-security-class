---
name: "dlopen"
type: function
introduced_in: ld_preload_0
introduced_date: 2026-02-03
scope: mastery
scope_history:
  - lecture: ld_preload_0
    date: 2026-02-03
  - lecture: reflective-payloads
    date: 2026-03-13
  - lecture: shellcode-loaders
    date: 2026-03-17
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - "[[dynamic-linking]]"
docs: "https://man7.org/linux/man-pages/man3/dlopen.3.html"
tags: [userland, linking, injection]
---

Opens a shared object and runs its constructors. Accepts RTLD_LAZY or RTLD_NOW for symbol resolution. In the reflective payload chain, resolved at runtime by walking libc's ELF symbol table when libc is not available at link time.

## Used In

- LD_PRELOAD hooking
- shellcode loaders (ex1-ex4)
- reflective-payloads.md

## See Also

- [[dlsym]]
- [[dlclose]]
- [[memfd-create]]
