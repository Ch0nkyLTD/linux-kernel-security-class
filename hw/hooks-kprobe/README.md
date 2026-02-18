# Guardian -- Kprobe

## Overview

In this assignment you'll write a kernel module that protects a directory using kprobes. The module does four things:

1. Blocks file access to `/tmp/secret` (bouncer)
2. Hides the directory from `ls` output (cloak)
3. Provides a bypass via a magic character device write (chardev)
4. Logs all access attempts to a ring buffer readable from userspace

The code is split across three `.c` files. They share state through a common header.

## Project Structure

```
guardian_kprobe_main.c   Part 1: Chardev + event log + credential modification
bouncer_kp.c            Part 2: Kprobe on __arm64_sys_openat (access blocking)
cloak_kp.c              Part 3: Kretprobe on __arm64_sys_getdents64 (file hiding)
guardian_kprobe.h        Shared header (PROVIDED -- do not modify)
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

This copies the module source into `modules/guardian_kprobe/` and the test script into `tests/`.

## Build and Test

From the lab directory:

```bash
make module-guardian_kprobe       # Cross-compile for arm64
make test-guardian_kprobe         # Build, boot VM, run tests, power off
```

The test boots a fresh VM every time. If your module panics the kernel, just run the test again. The VM is disposable.

---

## Part 1: Module Infrastructure -- `guardian_kprobe_main.c` (40 pts)

This file handles the character device, event logging, and credential modification. The hooks in Parts 2 and 3 call helpers defined here.

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

6. `guardian_init()` -- Module init with goto-based error unwinding:
   - `alloc_chrdev_region()` + `cdev_init()` + `cdev_add()`
   - `class_create()` + `device_create()` to make `/dev/guardian`
   - Call `bouncer_init()` and `cloak_init()`
   - Handle errors with goto unwinding (fail_cloak, fail_bouncer, fail_device, ...)

7. `guardian_exit()` -- Reverse teardown: cloak_exit, bouncer_exit, then destroy device/class/cdev/region.

### Key APIs

```c
in_group_p(kgid)                    // Check group membership
prepare_creds() / commit_creds()    // Credential modification
kfifo_put() / kfifo_get()           // Ring buffer operations
copy_from_user() / copy_to_user()   // Userspace data transfer
alloc_chrdev_region()               // Dynamic device number
class_create() / device_create()    // Auto /dev/ node via udev
```

---

## Part 2: Bouncer -- `bouncer_kp.c` (25 pts)

A kprobe on `__arm64_sys_openat` that denies access to `/tmp/secret` unless the caller has GID 1337.

### What to implement

1. `bouncer_pre_handler()` -- Kprobe entry handler. Fires every time `openat` is called.
   - Extract the filename from the syscall arguments
   - If it starts with `/tmp/secret` and the caller lacks GID 1337, deny access
   - Log the attempt via `guardian_log_event()`

2. `bouncer_init()` -- Register the kprobe on `__arm64_sys_openat`.

3. `bouncer_exit()` -- Unregister the kprobe.

### AArch64 double pt_regs

On AArch64, syscall wrappers like `__arm64_sys_openat` receive an outer `pt_regs` from the kprobe framework. Its `regs[0]` points to the inner (user-facing) `pt_regs`. The filename is in the inner regs:

```c
struct pt_regs *user_regs = (struct pt_regs *)regs->regs[0];
char __user *filename = (char __user *)user_regs->regs[1];
```

To deny access, zero the filename pointer. The kernel will return `EFAULT`:

```c
user_regs->regs[1] = 0;  // filename = NULL -> EFAULT
```

### Key APIs

```c
register_kprobe() / unregister_kprobe()
strncpy_from_user(kbuf, user_ptr, len)  // Safe string copy from userspace
strncmp(kbuf, PROTECTED_PATH, len)      // Path prefix match
```

---

## Part 3: Cloak -- `cloak_kp.c` (25 pts)

A kretprobe on `__arm64_sys_getdents64` that filters the directory listing buffer to hide `"secret"` from `/tmp/`.

### What to implement

1. `cloak_entry()` -- Kretprobe entry handler. Save the `dirp` pointer (the userspace buffer address) from the double pt_regs into the per-instance data.

2. `cloak_return()` -- Kretprobe return handler. After `getdents64` has filled the buffer:
   - Skip if the caller has GID 1337 (show everything)
   - Copy the dirent buffer from userspace
   - Walk the `linux_dirent64` records looking for `d_name == "secret"`
   - Remove matching entries by merging `d_reclen` values
   - Copy the filtered buffer back and update the return value

3. `cloak_init()` -- Register the kretprobe on `__arm64_sys_getdents64`.

4. `cloak_exit()` -- Unregister the kretprobe.

### Dirent filtering

`getdents64` returns a packed buffer of variable-length records. Each has a `d_reclen` field. To hide an entry:

- If it's not the first entry: add its `d_reclen` to the previous entry's `d_reclen` (this absorbs it)
- If it is the first entry: `memmove()` the remaining buffer forward and reduce the total byte count

After filtering, write the modified buffer back with `copy_to_user()` and set `regs->regs[0]` to the new byte count.

### Key APIs

```c
register_kretprobe() / unregister_kretprobe()
kmalloc(size, GFP_ATOMIC)          // Atomic -- we're in kretprobe context
copy_from_user() / copy_to_user()  // Buffer transfer
regs_return_value(regs)            // Get syscall return value
```

Kretprobe handlers run in atomic context. Use `GFP_ATOMIC`, not `GFP_KERNEL`.

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
cd modules/guardian_kprobe
make submission.zip
```

Upload `submission.zip` to Gradescope. The autograder cross-compiles your code, boots the same QEMU VM, and runs the same test script you see locally.

## Debugging Tips

**Check kernel messages.** Your `pr_info`/`pr_err`/`pr_warn` calls appear in `dmesg`. After `make test-guardian_kprobe`, look for lines prefixed with `guardian:`.

**Load manually.** Boot the VM and test interactively:

```bash
# From the lab directory
make module-guardian_kprobe
make modules-install
make shared

# Inside the VM
mount-shared
mkdir -p /tmp/secret && echo "test" > /tmp/secret/test.txt
insmod /mnt/shared/modules/guardian_kprobe.ko
ls /tmp/                      # "secret" should be hidden
cat /tmp/secret/test.txt      # should fail
printf '\xef\xbe\xad\xde' > /dev/guardian   # grant bypass
ls /tmp/                      # "secret" should appear
cat /tmp/secret/test.txt      # should work
cat /dev/guardian              # read event log
rmmod guardian_kprobe
```

**Use GDB.** Boot with `make shared` (starts with `-s -S`), then `make debug` in another terminal to step through your module.

**Common pitfalls:**

- Forgetting the double `pt_regs` indirection (you'll get garbage instead of filenames)
- Using `GFP_KERNEL` in kretprobe handlers (causes `sleeping function called from invalid context`)
- Not protecting kfifo access with the spinlock (race conditions under concurrent syscalls)
- Forgetting `groups_sort()` after appending the magic GID (group lookup may fail)
