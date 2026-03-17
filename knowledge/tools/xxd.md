---
name: "xxd"
type: tool
introduced_in: shellcode-loaders
introduced_date: 2026-03-17
scope: introduced
scope_history:
  - lecture: shellcode-loaders
    date: 2026-03-17
prerequisites:
  - (none)
docs: "https://man7.org/linux/man-pages/man1/xxd.1.html"
tags: [tool, binary]
---

`xxd -i` converts a binary file into a C array (`unsigned char[]`), enabling embedding of shared objects or shellcode directly in C source. Used in the shellcode loader exercises to embed the payload SO.

## Used In

- shellcode-loaders.md

## See Also

- [[reflective-loading]]
