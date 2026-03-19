# Reflective Memfd Payload

## Overview

In this assignment you'll build a reflective shared object that loads itself into a process entirely without touching the filesystem. The SO uses `memfd_create` to create an anonymous in-memory file, writes its own bytes into it, and then calls `dlopen("/proc/self/fd/N")` to load itself properly -- with relocations, symbol resolution, and `.init_array` constructors. This is the exact payload delivery mechanism used by kernel code injection modules.

The catch: the reflective loader runs inside a `-nostdlib` shared object mapped into raw RWX memory. There is no libc, no dynamic linker, no relocations yet. Every operation -- opening files, reading memory maps, resolving symbols -- must be done with raw syscalls, inline assembly, and manual ELF parsing.

You implement three files:

- **`loader.c`** -- Reflective loader entry point (`load_memfd()`)
- **`procfsutil.h`** -- Parse `/proc/self/maps` to find library base addresses
- **`elfsym.h`** -- Walk a mapped ELF's dynamic symbol table to resolve functions

Everything else is provided scaffolding.

## Project Structure

```
loader.c             Student-implemented: reflective loader entry point
procfsutil.h         Student-implemented: /proc/self/maps parser
elfsym.h             Student-implemented: ELF symbol table walker
payload.c            PROVIDED: constructor that creates proof file
test_harness.c       PROVIDED: RWX memory loader (simulates kernel injection)
bootstrap.py         PROVIDED: generates AArch64 stub that jumps into rp.so
syscall.h            PROVIDED: raw AArch64 syscall wrappers (svc #0)
minstr.h             PROVIDED: minimal string functions (strlen, strcmp, strstr, memcmp, hex_str_to_u64)
Makefile             PROVIDED: cross-compilation and payload assembly
```

## Setup

Run the install script to copy the starter files into your lab:

```bash
./install.sh
```

If your lab repo is not at `../aarch64-linux-qemu-lab`, pass its path:

```bash
./install.sh ~/path/to/aarch64-linux-qemu-lab
```

This copies the module source into `shared/reflective-memfd/` and the test script into `tests/`.

## Build

From the source directory:

```bash
cd shared/reflective-memfd
make
```

This cross-compiles:
1. `rp.so` -- Your reflective shared object (`-nostdlib -fPIC -shared`)
2. `bootstrap.bin` -- AArch64 stub that jumps into rp.so
3. `mempayload` -- bootstrap + rp.so concatenated (the final payload)
4. `test_harness` -- RWX memory loader that simulates kernel injection

## Test

Deploy and test in the lab VM:

```bash
# From the lab root directory
cp shared/reflective-memfd/mempayload shared/
cp shared/reflective-memfd/test_harness shared/
cp tests/test_reflective.sh shared/autorun.sh
chmod +x shared/autorun.sh shared/test_harness
rm -f debian-runtime.qcow2
make nodebug
```

Check `shared/test_results.log` after the VM powers off.

---

## `procfsutil.h` -- Parse `/proc/self/maps` (3 solution blocks)

This file implements `search_lib_procmaps()`, which finds a library's base address by reading `/proc/self/maps`. It takes a library name substring (e.g. `"libc.so"`), a permission string (e.g. `"r-xp"`), and an output pointer for the base address. Returns 0 on success, -1 on failure.

The function is split into three TODO blocks.

### Block 1: Open `/proc/self/maps`

Build the path `/proc/self/maps` on the stack using the provided `STACK_STR_PROC_SELF_MAPS()` macro, then open it with `sys_openat()`.

- Use `AT_FDCWD` (-100) as the directory file descriptor and `O_RDONLY` (0) as the flags
- Return -1 if the open fails (negative return value)

Why stack strings? This code runs inside a position-independent shared object loaded into raw memory without relocations. String literals normally go into `.rodata`, which requires relocation to access. Building strings character-by-character on the stack avoids any relocation dependency.

### Block 2: Read file contents

Read the file contents into the provided `buf[4096]` using `sys_read()`. Null-terminate the buffer (`buf[bytes_read] = '\0'`). Close the file descriptor with `sys_close()`. Return -1 if the read fails.

### Block 3: Line-by-line parse

Walk the buffer character by character. At each newline, process the line:

1. Temporarily null-terminate the line (save and restore the `'\n'`)
2. Find the first space -- everything before it is the address range (e.g. `7f8a1000-7f8a2000`)
3. Check if the permissions field (immediately after the first space) matches `prot` using `memcmp()`
4. Skip forward to the pathname column (after the 5th space from the permissions field)
5. Check if `lib_needle` appears in the pathname using `strstr()`
6. If both match, parse the hex start address from the beginning of the line using `hex_str_to_u64()`, store it in `*base_out`, and return 0

A `/proc/self/maps` line looks like:

```
7f8a1c0000-7f8a220000 r-xp 00000000 fe:01 262191   /usr/lib/aarch64-linux-gnu/libc.so.6
```

Key APIs (all provided in `syscall.h` and `minstr.h`): `sys_openat`, `sys_read`, `sys_close`, `strstr`, `memcmp`, `hex_str_to_u64`.

---

## `elfsym.h` -- Walk ELF Symbol Table (4 solution blocks)

This file implements `find_symbol_symtab()`, which resolves a function symbol from a mapped ELF shared object. Given a base address and a symbol name (e.g. `"dlopen"`), it returns the symbol's absolute address. This is how the loader finds `dlopen` and `sprintf` inside libc without using `dlsym`.

The function is split into four TODO blocks.

### Block 1: Verify ELF magic

Check that the first four bytes at `base` match the ELF magic number `\x7FELF`. Use `memcmp(base, ELFMAG, SELFMAG)`. Return NULL if the check fails.

### Block 2: Walk program headers to find `PT_DYNAMIC`

Cast `base` to an `Elf64_Ehdr *` to access the ELF header. The program header table starts at `base + ehdr->e_phoff` and contains `ehdr->e_phnum` entries. Each entry is an `Elf64_Phdr`. Iterate through them looking for one with `p_type == PT_DYNAMIC` (value 2).

When found, the dynamic section is at `base + phdr[i].p_vaddr`. Store it in `dynamic_section`.

### Block 3: Extract `DT_STRTAB`, `DT_SYMTAB`, `DT_STRSZ`

Walk the dynamic section entries (`Elf64_Dyn` array, terminated by `DT_NULL`). Extract three values:

| `d_tag` | Value | What it gives you |
|---------|-------|-------------------|
| `DT_STRTAB` | 5 | String table address (cast to `char *`) |
| `DT_SYMTAB` | 6 | Symbol table address (cast to `Elf64_Sym *`) |
| `DT_STRSZ` | 10 | String table size in bytes |

These are absolute addresses (already relocated by the kernel's ELF loader for libc).

### Block 4: Iterate symbol table entries

Walk the `Elf64_Sym` array starting from `symtab`. For each entry:

1. If `sym->st_name >= strtab_size`, you've gone past the end -- break
2. The symbol's name is at `strtab + sym->st_name`
3. Compare it against `symbol_name` using `strcmp()`
4. On match, return `(void *)((uintptr_t)base_addr + sym->st_value)`

The return value is `base + st_value` because `st_value` holds the symbol's offset from the ELF base.

Key types (from `<elf.h>`): `Elf64_Ehdr`, `Elf64_Phdr`, `Elf64_Dyn`, `Elf64_Sym`.

---

## `loader.c` -- Reflective Loader Entry Point (4 solution blocks)

This file implements the main reflective loading logic. The bootstrap stub branches into `load_memfd(size_t span)` with the total SO size in `x0`. The function finds its own ELF base, creates an anonymous memfd, resolves libc functions, writes itself to the memfd, and calls `dlopen` to trigger proper loading.

### Block 1: `find_base()` -- Scan backwards for ELF header

The `find_base()` helper function locates the SO's own ELF base address. Since the code is running from raw RWX memory, it doesn't know where it's loaded. The approach:

1. Take the address of `find_base` itself (a known address within the SO)
2. Mask off the low 12 bits to page-align it (`& ~0xFFF`)
3. Walk backwards page by page (up to 4 pages), checking if the first four bytes are `\x7FELF`
4. Return the address where the ELF magic is found, or NULL

This works because ELF files are always page-aligned and start with the ELF header.

### Block 2: Create memfd

Create an anonymous in-memory file using `sys_memfd_create()`. Build a short name on the stack (even a single character works). Pass `MFD_CLOEXEC` as the flags. Return NULL if it fails (negative return value).

`memfd_create` returns a file descriptor backed by anonymous memory. It appears in `/proc/self/fd/` but has no filesystem path -- perfect for fileless loading.

### Block 3: Resolve libc symbols

This is the core bootstrap problem: you need `dlopen` to load the SO properly, but `dlopen` lives in libc, and you have no dynamic linker. The solution is to find libc manually:

1. Use `search_lib_procmaps()` to find libc's base address. Search for `"libc.so"` with `"r-xp"` permissions (the executable mapping). Build both search strings on the stack using the provided `STACK_STR_LIBCSO()` and `STACK_STR_RXP()` macros.
2. Use `find_symbol_symtab()` to resolve `"dlopen"` from libc's ELF. Build the symbol name on the stack using `STACK_STR_DLOPEN()`.
3. Resolve `"sprintf"` the same way using `STACK_STR_SPRINTF()`.
4. Cast both to function pointers (`dlopen_func_t` and `sprintf_func_t`, both provided).
5. Return NULL if either resolution fails.

### Block 4: Write self to memfd and dlopen

With `dlopen` and `sprintf` resolved, complete the reflective load:

1. Build the path `"/proc/self/fd/<N>"` using `sprintf` with the memfd file descriptor number. Use the `STACK_STR_PROC_FD()` macro for the format string `"/proc/self/fd/%d"`.
2. Write the SO's bytes to the memfd using `sys_write(mem_fd, base, span)`. The `base` pointer (from `find_base()`) points to the start of the SO in memory, and `span` (passed in by bootstrap) is the total size.
3. Call `dlopen(path, RTLD_NOW)` (value 2). This makes the kernel load the SO from the memfd with full ELF loading -- relocations, symbol resolution, and `.init_array` constructors.

When `dlopen` succeeds, the constructor in `payload.c` fires automatically, creating the proof file.

Key APIs (from `syscall.h`): `sys_memfd_create`, `sys_write`.

---

## How It All Fits Together

The reflective loading chain has five stages:

```
test_harness
  |-- mmaps RWX memory
  |-- reads mempayload (bootstrap + rp.so) into it
  |-- jumps to offset 0
  v
bootstrap stub (4KB)
  |-- loads SO size into x0
  |-- branches forward into rp.so at load_memfd()
  v
load_memfd() [running in raw RWX memory, no libc]
  |-- find_base(): scans backwards for \x7FELF magic
  |-- sys_memfd_create(): creates anonymous fd
  |-- search_lib_procmaps(): reads /proc/self/maps, finds libc base
  |-- find_symbol_symtab(): walks libc's ELF to resolve dlopen + sprintf
  |-- sys_write(): copies SO bytes to memfd
  |-- dlopen("/proc/self/fd/N"): loads SO via kernel ELF loader
  v
dlopen triggers full ELF loading
  |-- relocations applied
  |-- .init_array processed
  v
payload_init() constructor fires
  |-- creates /tmp/payload_loaded_<pid> with "PAYLOAD_OK"
```

The key insight is that `load_memfd()` runs twice conceptually. The first time, it executes from raw memory without relocations -- it does just enough work (raw syscalls, manual ELF parsing) to get `dlopen` called. The second time, `dlopen` loads the same SO properly through the kernel's ELF loader, which handles relocations and calls constructors. The constructor in `payload.c` only runs in the second (properly loaded) instance.

---

## Grading (90 pts)

| Test | Points | What it checks |
|------|--------|----------------|
| Bootstrap assembled | 10 | `mempayload` exists and is > 4096 bytes |
| `find_base()` | 15 | Loader prints "found ELF base" |
| `search_lib_procmaps()` | 15 | Loader prints "found libc" |
| `find_symbol_symtab()` | 15 | Loader prints "loading via /proc/self/fd/..." |
| `memfd_create` | 10 | No "memfd_create failed" error |
| `dlopen` via memfd | 15 | Loader prints "[bootstrap] done" |
| Constructor proof | 10 | `/tmp/payload_loaded_<pid>` contains "PAYLOAD_OK" |
| **Total** | **90** |

Tests are cumulative -- each stage depends on the previous one succeeding. If `find_base()` fails, everything after it fails too.

## Submit to Gradescope

Upload three files: **`loader.c`**, **`procfsutil.h`**, and **`elfsym.h`**.

The autograder cross-compiles your code with the provided scaffolding, boots the same QEMU VM, and runs the same test script you see locally.

## Debugging Tips

**Check harness output.** The test harness prints diagnostic messages prefixed with `[bootstrap]` at each stage of the pipeline. Error messages are prefixed with `[-]`. Look at `shared/test_results.log` to see which stage failed.

**Test manually.** Boot the VM and run the harness interactively:

```bash
# Inside the VM after mounting shared:
mount-shared
cd /mnt/shared
chmod +x test_harness
./test_harness mempayload
```

**Inspect /proc/self/maps.** To understand what `search_lib_procmaps()` needs to parse:

```bash
cat /proc/self/maps
# Output lines look like:
# 7f8a1c0000-7f8a220000 r-xp 00000000 fe:01 262191  /usr/lib/.../libc.so.6
```

**Check ELF structure.** To understand the ELF layout your parser walks:

```bash
readelf -h /lib/aarch64-linux-gnu/libc.so.6     # ELF header (e_phoff, e_phnum)
readelf -l /lib/aarch64-linux-gnu/libc.so.6     # Program headers (find PT_DYNAMIC)
readelf -d /lib/aarch64-linux-gnu/libc.so.6     # Dynamic section (DT_STRTAB, DT_SYMTAB)
readelf --dyn-syms /lib/aarch64-linux-gnu/libc.so.6 | grep dlopen  # Verify symbol exists
```

**Common pitfalls:**
- Forgetting to page-align the address in `find_base()` (mask with `& ~0xFFF`, not `& ~0xFF`)
- Using string literals instead of stack strings (causes relocation-dependent `.rodata` references that crash in raw memory)
- Forgetting to null-terminate the buffer after `sys_read()` in `search_lib_procmaps()`
- Off-by-one in the `/proc/self/maps` line parser (the pathname is after the 5th space from the permissions field, not from the start of the line)
- Confusing `DT_STRTAB`/`DT_SYMTAB` values as offsets vs. absolute addresses (in libc's case they are absolute addresses set by the dynamic linker)
- Not checking the `strtab_size` bound when iterating the symbol table (causes reads past the end of the string table)
- Writing `span` bytes starting from the wrong base pointer (use the `base` from `find_base()`, not some other address)
