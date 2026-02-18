# Guardian -- Ftrace

## Overview

In this assignment you'll write a kernel module that protects a directory using ftrace, the kernel's function tracing framework. The module does four things:

1. Blocks file access to `/tmp/secret` (bouncer)
2. Hides the directory from `ls` output (cloak)
3. Provides a bypass via a magic character device write (chardev)
4. Logs all access attempts to a ring buffer readable from userspace

The code is split across three `.c` files. They share state through a common header.

## Project Structure

```
guardian_ftrace_main.c   Part 1: Chardev + event log + credential modification
bouncer_ft.c            Part 2: Ftrace hook on do_sys_openat2 (access blocking)
cloak_ft.c              Part 3: Ftrace IP redirect on __arm64_sys_getdents64 (file hiding)
guardian_ftrace.h        Shared header (PROVIDED -- do not modify)
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

This copies the module source into `modules/guardian_ftrace/` and the test script into `tests/`.

## Build and Test

From the lab directory:

```bash
make module-guardian_ftrace       # Cross-compile for arm64
make test-guardian_ftrace         # Build, boot VM, run tests, power off
```

The test boots a fresh VM every time. If your module panics the kernel, just run the test again. The VM is disposable.

---

## Part 1: Module Infrastructure -- `guardian_ftrace_main.c` (40 pts)

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

## Part 2: Bouncer -- `bouncer_ft.c` (25 pts)

An ftrace callback on `do_sys_openat2` that denies access to `/tmp/secret` unless the caller has GID 1337.

### What to implement

1. `bouncer_callback()` -- Ftrace callback function. Fires every time `do_sys_openat2` is called.
   - Extract the filename using `ftrace_regs_get_argument(fregs, 1)` -- argument 1 is the filename pointer
   - If it starts with `/tmp/secret` and the caller lacks GID 1337, deny access
   - Log the attempt via `guardian_log_event()`

2. `bouncer_init()` -- Set up the ftrace hook:
   - Resolve `do_sys_openat2` address using the kprobe bootstrap trick
   - `ftrace_set_filter_ip()` to tell ftrace which function to hook
   - `register_ftrace_function()` to activate the callback

3. `bouncer_exit()` -- Unregister the ftrace function, then clear the filter.

### No double pt_regs

Unlike the kprobe variant, ftrace on `do_sys_openat2` gives you direct access to function arguments via `ftrace_regs_get_argument()`. There is no double `pt_regs` indirection -- `do_sys_openat2` is an internal kernel function, not the `__arm64_sys_*` wrapper.

```c
const char __user *filename = (const char __user *)
    ftrace_regs_get_argument(fregs, 1);
```

To deny access, zero the filename register directly:

```c
fregs->regs[1] = 0;  // filename = NULL -> EFAULT
```

### Ftrace ops flags

The `ftrace_ops` struct needs specific flags:

```c
static struct ftrace_ops bouncer_ops = {
    .func  = bouncer_callback,
    .flags = FTRACE_OPS_FL_IPMODIFY |
             FTRACE_OPS_FL_RECURSION,
};
```

- `FL_IPMODIFY` -- we modify register state (the filename pointer)
- `FL_RECURSION` -- built-in recursion protection
- Do not set `FL_SAVE_REGS` -- AArch64 doesn't support it

### Key APIs

```c
ftrace_regs_get_argument(fregs, n)     // Get function argument n
ftrace_set_filter_ip(&ops, addr, 0, 0) // Set IP filter
register_ftrace_function(&ops)          // Activate callback
unregister_ftrace_function(&ops)        // Deactivate callback
strncpy_from_user(kbuf, user_ptr, len)  // Safe string copy from userspace
```

---

## Part 3: Cloak -- `cloak_ft.c` (25 pts)

An ftrace IP redirect on `__arm64_sys_getdents64` that filters the directory listing buffer to hide `"secret"` from `/tmp/`.

### What to implement

1. `guardian_getdents64_wrapper()` -- Replacement function that runs instead of the original `getdents64`. This is where the filtering logic lives:
   - Call the original `getdents64` handler via the saved function pointer
   - Clear the per-CPU `cloak_active` flag after the original returns (re-enables the hook for the next syscall)
   - Skip filtering if the caller has GID 1337 (show everything)
   - Copy the dirent buffer from userspace
   - Walk the linked `linux_dirent64` records looking for `d_name == "secret"`
   - Remove matching entries by merging `d_reclen` values
   - Copy the filtered buffer back and return the new byte count

2. `cloak_callback()` -- Ftrace callback. Check the per-CPU `cloak_active` flag to prevent infinite recursion, then set it and redirect the instruction pointer to the wrapper using `ftrace_regs_set_instruction_pointer()`.

3. `cloak_init()` -- Set up the ftrace hook:
   - Resolve `__arm64_sys_getdents64` and save it as the `original_getdents64` pointer
   - `ftrace_set_filter_ip()` + `register_ftrace_function()`

4. `cloak_exit()` -- Unregister the ftrace function and clear the filter.

### IP redirect technique

The IP redirect is the ftrace equivalent of a kretprobe -- it lets you run code after the original function. Instead of hooking the return, you redirect execution to a wrapper that calls the original, then post-processes the result:

```
Entry -> ftrace fires -> callback checks per-CPU guard -> sets guard -> redirects IP
  |
wrapper runs
  +-- calls original_getdents64()
  |     +-- ftrace fires again -> per-CPU guard active -> skip (no redirect)
  +-- clears per-CPU guard
  +-- filters the dirent buffer
  +-- returns modified byte count
```

When the wrapper calls `original_getdents64()`, execution re-enters `__arm64_sys_getdents64` and the ftrace callback fires again. Without the guard, this causes infinite recursion (stack overflow). The per-CPU flag tells the callback to skip the redirect on re-entry:

```c
static DEFINE_PER_CPU(int, cloak_active);

/* In the callback: */
if (this_cpu_read(cloak_active))
    return;                            // re-entry -- let original run
this_cpu_write(cloak_active, 1);
ftrace_regs_set_instruction_pointer(fregs, (unsigned long)wrapper);

/* In the wrapper, after calling original: */
this_cpu_write(cloak_active, 0);       // re-enable for next syscall
```

### Dirent filtering

`getdents64` returns a packed buffer of variable-length records. Each has a `d_reclen` field. To hide an entry:

- If it's not the first entry: add its `d_reclen` to the previous entry's `d_reclen` (this absorbs it)
- If it is the first entry: `memmove()` the remaining buffer forward and reduce the total byte count

After filtering, write the modified buffer back with `copy_to_user()` and return the new `total_len`.

### Key APIs

```c
ftrace_regs_set_instruction_pointer(fregs, addr)  // Redirect execution
DEFINE_PER_CPU(int, cloak_active)                  // Per-CPU recursion guard
this_cpu_read() / this_cpu_write()                 // Per-CPU access
kmalloc(size, GFP_KERNEL)                          // Process context -- GFP_KERNEL is fine
copy_from_user() / copy_to_user()                  // Buffer transfer
original_getdents64(regs)                          // Call saved original handler
```

Unlike kretprobe handlers, the wrapper runs in normal process context (it replaced the original function). `GFP_KERNEL` is safe here.

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
cd modules/guardian_ftrace
make submission.zip
```

Upload `submission.zip` to Gradescope. The autograder cross-compiles your code, boots the same QEMU VM, and runs the same test script you see locally.

## Debugging Tips

**Check kernel messages.** Your `pr_info`/`pr_err`/`pr_warn` calls appear in `dmesg`. After `make test-guardian_ftrace`, look for lines prefixed with `guardian:`.

**Load manually.** Boot the VM and test interactively:

```bash
# From the lab directory
make module-guardian_ftrace
make modules-install
make shared

# Inside the VM
mount-shared
mkdir -p /tmp/secret && echo "test" > /tmp/secret/test.txt
insmod /mnt/shared/modules/guardian_ftrace.ko
ls /tmp/                      # "secret" should be hidden
cat /tmp/secret/test.txt      # should fail
printf '\xef\xbe\xad\xde' > /dev/guardian   # grant bypass
ls /tmp/                      # "secret" should appear
cat /tmp/secret/test.txt      # should work
cat /dev/guardian              # read event log
rmmod guardian_ftrace
```

**Use GDB.** Boot with `make shared` (starts with `-s -S`), then `make debug` in another terminal to step through your module.

**Common pitfalls:**
- Forgetting the per-CPU recursion guard in the cloak (causes kernel stack overflow -- infinite `wrapper -> original -> ftrace -> wrapper`)
- Forgetting to clear `cloak_active` after calling the original in the wrapper (hook stays disabled forever)
- Forgetting `ftrace_set_filter_ip()` before `register_ftrace_function()` (hooks every function in the kernel)
- Setting `FTRACE_OPS_FL_SAVE_REGS` (not supported on AArch64 -- will fail to register)
- Using `ftrace_regs_get_argument()` in the cloak callback (the cloak hooks a syscall wrapper, not an internal function -- use IP redirect instead)
- Not protecting kfifo access with the spinlock (race conditions under concurrent syscalls)
- Forgetting `groups_sort()` after appending the magic GID (group lookup may fail)
