---
name: "dlsym"
type: function
introduced_in: ld_preload_0
introduced_date: 2026-02-03
scope: used
scope_history:
  - lecture: ld_preload_0
    date: 2026-02-03
  - lecture: reflective-payloads
    date: 2026-03-13
  - lecture: capstone-kernel
    date: 2026-02-24
prerequisites:
  - "[[dlopen]]"
docs: "https://man7.org/linux/man-pages/man3/dlsym.3.html"
tags: [userland, linking, hooking]
---

Looks up a symbol in a shared object handle. With RTLD_NEXT, finds the next occurrence in the search order -- the core mechanism for LD_PRELOAD interception where a wrapper calls through to the real function.

## Used In

- LD_PRELOAD libraries (HW3)
- capstone Part A

## See Also

- [[dlopen]]
- [[ld-preload]]
