# AArch64 LD_PRELOAD + Syscall Bypass Assignment

## Assignment Overview

This assignment explores two userland interception techniques and one bypass technique:

1. **Passive LD_PRELOAD shim**: override `open/open64/openat/unlink/unlinkat` by symbol name.
2. **Active GOT/PLT patching**: locate the `openat` / `unlinkat` relocations in the main executable and overwrite their GOT entries.
3. **PLT stub patching**: locate the PLT stub for `openat` / `unlinkat` and overwrite its instructions to jump to your detour.
4. **Syscall bypass**: use **direct syscalls** to write a protected file even when a teacher-provided `hook.so` is preloaded.

**Protected directory**: `/tmp/protected`

---

## What You Must Implement

### Part 1: `binding_hook.c` (Passive LD_PRELOAD Shim)

Implement a shared library that blocks access to `/tmp/protected/...` by overriding the following **symbols**:

- `open`
- `open64`
- `openat`
- `unlink`
- `unlinkat`

**Requirements:**

- For any path under `/tmp/protected`, return `-1` and set `errno = EACCES`.
- For any other path, call the real libc function using `dlsym(RTLD_NEXT, ...)`.

### Part 2: `active_hook.c` (Active GOT/PLT Patching)

**Comparison to 3:** this method **overwrites the GOT jump slot** for each symbol. The PLT stub stays intact; it simply reads the new GOT address.

Implement a shared library that patches the main executable’s GOT entries for:

- `openat`
- `unlinkat`

**Requirements:**

- read `man dl_iterate_phdr`
- Use `dl_iterate_phdr` to find the main executable.
- Parse the dynamic segment to locate `DT_JMPREL` relocations.
- Use `mprotect` to make the GOT writable.
- Overwrite the GOT entry with your own detour functions.
- Deny access to `/tmp/protected/...` similarly to Part 1.

**Inspection tips (GOT):**

```bash
readelf -W -d bin/tester | grep -E 'JMPREL|SYMTAB|STRTAB|PLTRELSZ'
readelf -W -r bin/tester | grep -E 'JUMP_SLOT'
readelf -W -s bin/tester | grep -E 'openat|unlinkat'
```

**Relevant structs (elf.h / link.h):**

- `Elf64_Dyn`, `Elf64_Rela`, `Elf64_Sym`, `Elf64_Phdr`, `struct dl_phdr_info`

### Part 3: `plt_hook.c` (PLT Stub Patching)

**Comparison to 2:** this method **overwrites the PLT stub instructions themselves**. The GOT entry is untouched, but calls never reach it because the PLT code jumps directly to your detour.

Implement a shared library that patches the **PLT stub instructions** for:

- `openat`
- `unlinkat`

**Requirements:**

- Find the PLT entry for each target symbol.
- Overwrite the PLT stub instructions to jump directly to your detour.
- Deny access to `/tmp/protected/...` similarly to Part 1.

**Inspection tips (PLT):**

```bash
readelf -W -S bin/tester | grep -E '\\.plt'
readelf -W -r bin/tester | grep -E 'JUMP_SLOT'
aarch64-linux-gnu-objdump -d -j .plt bin/tester
```

**Relevant structs (elf.h):**

- `Elf64_Ehdr`, `Elf64_Shdr`, `Elf64_Rela`, `Elf64_Sym`

### Part 4: `bypass_syscall.c` (Direct Syscall Bypass)

Write a **syscall-only** program that creates or overwrites:

```
/tmp/protected/hacked.txt
```

with the exact contents:

```
1337h4x0r
```

**Requirements:**

- Must use **direct syscalls** (`openat`, `read`, `write`, `close`).
- Must use your syscall framework from the previous assignment.
- The autograder runs this binary with a teacher-provided `hook.so` via `LD_PRELOAD`.
- Your program must still succeed and write the correct payload.
  
Note: `bypass_syscall` is built as a normal dynamic ELF, but it still uses direct
syscalls instead of libc wrappers, so LD_PRELOAD hooks do not intercept it.

---

## Build

```bash
cd sample_submission
make all CROSS_COMPILE=aarch64-linux-gnu-
```

This produces:

- `bin/libshim.so`
- `bin/libgotpatch.so`
- `bin/libplthook.so`
- `bin/tester`
- `bin/bypass_syscall`

---

## Run (Local)

### Passive Shim

```bash
make run-passive CROSS_COMPILE=aarch64-linux-gnu-
```

### Active GOT Patch

```bash
make run-active CROSS_COMPILE=aarch64-linux-gnu-
```

### PLT Stub Patch

```bash
make run-plt CROSS_COMPILE=aarch64-linux-gnu-
```

### Syscall Bypass

```bash
make run-bypass CROSS_COMPILE=aarch64-linux-gnu-
```

If your ARM64 sysroot lives elsewhere, override the loader prefix, e.g.:

```bash
make run-passive CROSS_COMPILE=aarch64-linux-gnu- QEMU_LD=/opt/aarch64-rootfs
```

---

## Submission

Create a zip:

```bash
make submission.zip
```

Upload `submission.zip` to Gradescope.

---

## Notes on Prior Assignment Dependencies

The files `inc/syscall_utils.h`, `inc/minc.h`, and `src/minc.c` come from the **AArch64 File I/O Syscall Assignment**.
They include solution blocks and are stripped in the student starter pack. You are expected to bring your completed versions from that assignment.

---
