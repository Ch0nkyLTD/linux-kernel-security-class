# Guardian -- Syscall Table

## Overview

In this assignment you'll write a kernel module that protects a directory by patching the syscall table directly -- the kernel equivalent of GOT/PLT hooking in userspace. The module does four things:

1. Blocks file access to `/tmp/secret` (bouncer)
2. Hides the directory from `ls` output (cloak)
3. Provides a bypass via a magic character device write (chardev)
4. Logs all access attempts to a ring buffer readable from userspace

The code is split across three `.c` files. They share state through a common header.

## Project Structure

```
guardian_table_main.c   Part 1: Chardev + event log + credentials + PTE utilities
bouncer_tbl.c           Part 2: Replacement openat handler (access blocking)
cloak_tbl.c             Part 3: Replacement getdents64 handler (file hiding)
guardian_table.h         Shared header (PROVIDED -- do not modify)
```

## Setup

You need the `aarch64-linux-qemu-lab` already set up and working (kernel built, rootfs created, VM boots).

Run the install script to copy the starter files into your lab:

```bash
./install.sh
```

If your lab repo is not at `../aarch64-linux-qemu-lab`, pass its path:

```bash
./install.sh ~/path/to/aarch64-linux-qemu-lab
```

This copies the module source into `modules/guardian_table/` and the test script into `tests/`.

## Build and Test

From the lab directory:

```bash
make module-guardian_table       # Cross-compile for arm64
make test-guardian_table         # Build, boot VM, run tests, power off
```

The test boots a fresh VM every time. If your module panics the kernel, just run the test again. The VM is disposable.

---

## Part 1: Module Infrastructure -- `guardian_table_main.c` (40 pts)

This file handles the character device, event logging, credential modification, symbol resolution, and PTE manipulation. The hook functions in Parts 2 and 3 are simple -- the complexity lives here.

### What to implement

1. `guardian_has_magic_gid()` -- Check if the current process has GID 1337. Use `in_group_p()` with `KGIDT_INIT()`.

2. `guardian_log_event()` -- Record an access attempt in the kfifo ring buffer. Fill in a `struct guardian_event` with timestamp (`ktime_get_ns()`), PID, UID, path, comm, and the allowed/denied flag. Protect the kfifo with the spinlock.

3. `grant_magic_gid()` -- Add GID 1337 to the calling process's credentials:
   - `prepare_creds()` to copy current credentials
   - `groups_alloc(ngroups + 1)` to create a new group list
   - Copy existing groups, append the magic GID, then `groups_sort()`
   - `set_groups()` + `commit_creds()` to apply

4. `guardian_write()` -- Chardev write handler. Read exactly 4 bytes from userspace. If the value equals `0xDEADBEEF`, call `grant_magic_gid()`.

5. `guardian_read()` -- Chardev read handler. Dequeue one event from the kfifo, format it as a human-readable line, and copy to userspace.

6. `resolve_pte()` -- Walk the kernel page tables to find the PTE mapping a given virtual address. Walk through all four levels: PGD -> P4D -> PUD -> PMD -> PTE. Handle section mappings (1GB PUD sections and 2MB PMD sections) by returning early with the appropriate level.

7. `set_table_rw()` -- Make the syscall table page writable:
   - Call `resolve_pte()` on `sys_call_table_ptr`
   - Save the original PTE
   - Clear `PTE_RDONLY` (AArch64's hardware read-only bit)
   - `flush_tlb_all()` to ensure the TLB picks up the change

8. `set_table_ro()` -- Restore read-only protection by writing back the saved PTE and flushing the TLB.

9. `guardian_init()` -- Module init. This is the most complex init of the three variants:
   - Bootstrap symbol resolution: use the kprobe trick to find `kallsyms_lookup_name`, then use it to find `sys_call_table` and `init_mm`
   - Create the character device (same as other variants)
   - Save original handlers: `sys_call_table[__NR_openat]` and `sys_call_table[__NR_getdents64]`
   - Get hook pointers from `bouncer_get_hook()` and `cloak_get_hook()`
   - Open a single RW window: `set_table_rw()`, patch both entries, `set_table_ro()`
   - Handle errors with goto unwinding

10. `guardian_exit()` -- Restore both syscall table entries in a single RW window, then destroy the character device.

### APIs

```c
in_group_p(kgid)                    // Check group membership
prepare_creds() / commit_creds()    // Credential modification
kfifo_put() / kfifo_get()           // Ring buffer operations
copy_from_user() / copy_to_user()   // Userspace data transfer
alloc_chrdev_region()               // Dynamic device number
class_create() / device_create()    // Auto /dev/ node via udev
pgd_offset() / p4d_offset() /
pud_offset() / pmd_offset() /
pte_offset_kernel()                 // Page table walk
pud_sect() / pmd_sect()             // Check for section mappings
flush_tlb_all()                     // TLB invalidation
```

### Page table walk

The syscall table lives in `.rodata`, which is mapped read-only via `CONFIG_STRICT_KERNEL_RWX`. To patch it, you walk the kernel page tables starting from `init_mm`:

```
PGD -> P4D -> PUD -> PMD -> PTE
```

At each level, check for `none` (unmapped) entries. At PUD and PMD levels, also check for section mappings (`pud_sect()` / `pmd_sect()`) -- the kernel may use 1GB or 2MB huge pages instead of 4KB pages:

```c
if (pud_sect(READ_ONCE(*pudp))) {
    *levelp = 1;
    return (pte_t *)pudp;  // PUD section -- cast to pte_t*
}
```

Once you have the PTE, clear the read-only bit:

```c
new = __pte(pte_val(saved_pte) & ~PTE_RDONLY);
WRITE_ONCE(*saved_ptep, new);
flush_tlb_all();
```

---

## Part 2: Bouncer -- `bouncer_tbl.c` (25 pts)

A replacement handler for `sys_call_table[__NR_openat]` that denies access to `/tmp/secret` unless the caller has GID 1337.

### What to implement

1. `hooked_openat()` -- Replacement syscall handler. Receives `const struct pt_regs *regs` directly from the syscall dispatcher.
   - Extract the filename from `regs->regs[1]` (no double indirection -- you are the syscall handler)
   - If it starts with `/tmp/secret` and the caller lacks GID 1337, return `-EACCES`
   - Otherwise, call `original_openat(regs)` to pass through to the real handler
   - Log the attempt via `guardian_log_event()`

2. `bouncer_get_hook()` -- Already provided. Returns the `hooked_openat` function pointer so `guardian_init()` can patch the table.

### Direct pt_regs -- no double indirection

Because your hook replaces the `__arm64_sys_openat` entry in the syscall table, you are the syscall wrapper. The `regs` pointer you receive is the real user-facing `pt_regs`:

```c
char __user *filename = (char __user *)regs->regs[1];
```

To deny access, return `-EACCES` directly -- much cleaner than the kprobe/ftrace approach of zeroing registers:

```c
return -EACCES;    // Clean denial
```

For allowed requests, delegate to the original:

```c
return original_openat(regs);
```

### Key APIs

```c
strncpy_from_user(kbuf, user_ptr, len)  // Safe string copy from userspace
strncmp(kbuf, PROTECTED_PATH, len)      // Path prefix match
original_openat(regs)                    // Call saved original handler
```

---

## Part 3: Cloak -- `cloak_tbl.c` (25 pts)

A replacement handler for `sys_call_table[__NR_getdents64]` that filters the directory listing buffer to hide `"secret"` from `/tmp/`.

### What to implement

1. `hooked_getdents64()` -- Replacement syscall handler:
   - Call `original_getdents64(regs)` first to get the real directory listing
   - Skip filtering if the caller has GID 1337 (show everything)
   - Copy the dirent buffer from userspace (`regs->regs[1]` is the `dirp` pointer)
   - Walk the linked `linux_dirent64` records looking for `d_name == "secret"`
   - Remove matching entries by merging `d_reclen` values
   - Copy the filtered buffer back and return the new byte count

2. `cloak_get_hook()` -- Already provided. Returns the `hooked_getdents64` function pointer.

### Dirent filtering

`getdents64` returns a packed buffer of variable-length records. Each has a `d_reclen` field. To hide an entry:

- If it's not the first entry: add its `d_reclen` to the previous entry's `d_reclen` (this absorbs it)
- If it is the first entry: `memmove()` the remaining buffer forward and reduce the total byte count

After filtering, write the modified buffer back with `copy_to_user()` and return the new `total_len`.

### Key APIs

```c
kmalloc(size, GFP_KERNEL)           // Process context -- GFP_KERNEL is fine
copy_from_user() / copy_to_user()   // Buffer transfer
original_getdents64(regs)           // Call saved original handler
```

Your replacement handler runs in normal process context (you replaced the syscall entry). `GFP_KERNEL` is safe here.

---

## Grading (100 pts)

| Component | Points |
|-----------|--------|
| Compilation (automatic) | 10 |
| `insmod` succeeds | 10 |
| `/dev/guardian` exists | 5 |
| Load message in dmesg | 5 |
| `ls /tmp/` hides "secret" | 15 |
| `cat /tmp/secret/test.txt` denied | 15 |
| Magic bytes grant GID 1337 | 10 |
| `ls` shows "secret" after bypass | 10 |
| `cat` works after bypass | 10 |
| Event log returns entries | 5 |
| `rmmod` succeeds | 5 |
| **Total** | **100** |

## Submit to Gradescope

From your module directory:

```bash
cd modules/guardian_table
make submission.zip
```

Upload `submission.zip` to Gradescope. The autograder cross-compiles your code, boots the same QEMU VM, and runs the same test script you see locally.

## Debugging Tips

**Check kernel messages.** Your `pr_info`/`pr_err`/`pr_warn` calls appear in `dmesg`. After `make test-guardian_table`, look for lines prefixed with `guardian:`.

**Load manually.** Boot the VM and test interactively:

```bash
# From the lab directory
make module-guardian_table
make modules-install
make shared

# Inside the VM
mount-shared
mkdir -p /tmp/secret && echo "test" > /tmp/secret/test.txt
insmod /mnt/shared/modules/guardian_table.ko
ls /tmp/                      # "secret" should be hidden
cat /tmp/secret/test.txt      # should fail
printf '\xef\xbe\xad\xde' > /dev/guardian   # grant bypass
ls /tmp/                      # "secret" should appear
cat /tmp/secret/test.txt      # should work
cat /dev/guardian              # read event log
rmmod guardian_table
```

**Use GDB.** Boot with `make shared` (starts with `-s -S`), then `make debug` in another terminal to step through your module.

**Common issues:**

- Forgetting to `flush_tlb_all()` after modifying the PTE (stale TLB entry still maps read-only)
- Not restoring the syscall table on `rmmod` (leaves dangling pointers -- next openat call panics)
- Using `kallsyms_lookup_name()` directly (not exported since 5.7 -- use the kprobe bootstrap trick)
- Confusing PUD/PMD section mappings with regular page mappings (check `pud_sect()`/`pmd_sect()`)
- Not protecting kfifo access with the spinlock (race conditions under concurrent syscalls)
- Forgetting `groups_sort()` after appending the magic GID (group lookup may fail)
