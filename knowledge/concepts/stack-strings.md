---
name: "stack strings"
type: concept
introduced_in: shellcode-loaders
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: shellcode-loaders
    date: 2026-03-17
prerequisites:
  - (none)
docs: ""
tags: [userland, stealth, shellcoding]
---

Technique of building strings character-by-character on the stack at runtime instead of using string literals in `.rodata`. Prevents static analysis tools (`strings`) from seeing what paths or symbols the binary references. Used in no-libc reflective loaders.

## Used In

- shellcode-loaders.md (ex3, ex4)

## See Also

- [[reflective-loading]]
