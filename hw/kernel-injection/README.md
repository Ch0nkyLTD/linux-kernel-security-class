# Kernel Process Injection

## Overview

In this assignment you'll write a kernel module that injects code into a running process using progressively stealthier techniques. The module exposes a character device (`/dev/kinject`) that accepts four commands:

1. **`inject1 <pid>`** -- Allocate an RWX page via `vm_mmap()`, write shellcode, hijack the target's PC directly. The target is consumed -- its original behavior is destroyed.
2. **`inject2 <pid>`** -- Same `vm_mmap` approach, but use a clone() trampoline so the target survives. Parent resumes normally, child runs the payload.
3. **`scan <pid>`** -- Walk the target's AArch64 4-level page tables and report valid PTEs, flagging any with no backing VMA (anomalies).
4. **`inject_pte <pid>`** -- Manually construct page table entries for code and stack pages with no VMA backing, then spawn a clone thread. Invisible to `/proc/pid/maps`.

Part 5 is an integration test: after `inject_pte`, run `scan` on the same process. Your scanner should detect the stealth injection as anomalous PTEs. No new code needed -- this tests that Parts 3 and 4 work together.

All commands operate on a sleeping target process (e.g. `sleep 9999 &`). The injected shellcode creates a proof file at `/tmp/kinject1_<pid>`, `/tmp/kinject2_<pid>`, or `/tmp/kinject_pte_<pid>`.

The code lives in a single file: `kinject.c`. The instruction encoding helpers, shellcode builder, clone trampoline builder, VMA helpers, character device, and module init/exit are all provided. You implement the core logic in the TODO blocks.

## Project Structure

```
kinject.c       All parts -- single file with TODO blocks
Makefile        Build rules (provided)
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

This copies the module source into `modules/kinject/` and the test script into `tests/`.

## Build and Test

From the lab directory:

```bash
make module-kinject       # Cross-compile for arm64
make test-kinject         # Build, boot VM, run tests, power off
```

The test boots a fresh VM every time. If your module panics the kernel, just run the test again. The VM is disposable.

---

## Part 1: Direct PC Hijack -- `vma_inject_worker()` + `do_inject1()` (15 pts)

Inject shellcode into a target process by allocating an RWX page with `vm_mmap()`, writing the payload with `copy_to_user()`, then hijacking the target's saved PC so it runs the payload directly. The target process is consumed -- it becomes the payload.

### Background: Why a workqueue?

`vm_mmap()` always operates on `current->mm`. To create a mapping in the *target's* address space, you need `current->mm` to be the target's mm. The function `kthread_use_mm()` does exactly this -- it temporarily adopts another process's mm -- but it requires `current->mm == NULL`, which is only true for kernel threads.

Workqueue workers are kernel threads. The provided scaffolding schedules a `work_struct` that calls your code inside a worker. By the time your TODO block runs, `kthread_use_mm(target->mm)` has already been called, so `current->mm` is the target's mm. `vm_mmap()` and `copy_to_user()` will operate on the target's address space.

### What to implement

There are two TODO blocks.

**TODO Block 1 (in `vma_inject_worker`):** Allocate and populate the injected page.

- Call `vm_mmap(NULL, 0, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0)` to allocate one RWX page
- Check the return with `IS_ERR_VALUE()` -- on failure, set `ctx->result`, call `kthread_unuse_mm(mm)` and `mmdrop(mm)`, call `complete(&ctx->done)`, and return
- Call `build_payload(payload_buf, inject_addr, ctx->prefix, ctx->target_pid)` to generate shellcode
- Call `copy_to_user((void __user *)inject_addr, payload_buf, payload_len * sizeof(u32))` to write it
- On `copy_to_user` failure, set `ctx->result = -EFAULT`, clean up, and return

**TODO Block 2 (in `do_inject1`):** Hijack the target's PC and wake it.

- `regs = task_pt_regs(task)` to get the saved register state
- `regs->pc = ctx.inject_addr` to redirect execution
- `regs->syscallno = -1` to prevent syscall restart (which would overwrite your PC)
- `set_tsk_thread_flag(task, TIF_SIGPENDING)` to force the sleeping syscall to return
- `wake_up_process(task)` to wake the target

### Key APIs

```c
vm_mmap(file, addr, len, prot, flags, offset)   // Create VMA-backed mapping
IS_ERR_VALUE(addr)                               // Check for error return
copy_to_user(dst, src, len)                      // Write to userspace
build_payload(buf, va, prefix, pid)              // Generate shellcode (provided)
task_pt_regs(task)                               // Get saved register state
set_tsk_thread_flag(task, TIF_SIGPENDING)        // Force signal check
wake_up_process(task)                            // Wake sleeping task
```

---

## Part 2: Clone Trampoline -- `do_inject2()` (15 pts)

Same vm_mmap approach as Part 1 for memory allocation (reuses the same worker), but now uses a clone() trampoline so the target process survives. The trampoline calls `clone()` to spawn a child thread: the parent resumes at its original PC, and the child runs the payload.

### What to implement

There are two TODO blocks in `do_inject2()`. The scaffolding handles task lookup, the vm_mmap worker (reusing Part 1's), and building the clone trampoline.

**TODO Block 1:** Write the clone trampoline into an existing VM_EXEC page.

- `find_exec_addr(task, tramp_len * sizeof(u32))` to find a suitable region (returns 0 on failure)
- `access_process_vm(task, exec_addr, trampoline_buf, tramp_len * sizeof(u32), FOLL_WRITE | FOLL_FORCE)` to write the trampoline. `FOLL_FORCE` triggers COW on read-only executable pages.
- Check that the return value equals the expected byte count

**TODO Block 2:** Hijack the target's PC with clone registers.

- `regs = task_pt_regs(task)`
- `regs->regs[28] = regs->pc` -- save the original PC (parent will `br x28` to resume)
- `regs->regs[27] = ctx.inject_addr` -- the payload VA (child will `br x27`)
- `regs->pc = exec_addr` -- redirect to the clone trampoline
- `regs->syscallno = -1` -- prevent syscall restart
- `set_tsk_thread_flag(task, TIF_SIGPENDING)` + `wake_up_process(task)`

### Key APIs

```c
find_exec_addr(task, need)                      // Find VM_EXEC page (provided)
access_process_vm(task, addr, buf, len, flags)  // Read/write target memory
FOLL_WRITE | FOLL_FORCE                         // Force COW, write access
task_pt_regs(task)                              // Get saved register state
```

---

## Part 3: Page Table Walking -- `scan_ptes()` (20 pts)

Walk a target process's AArch64 4-level page tables (PGD -> P4D -> PUD -> PMD -> PTE) and report all valid user PTEs. Flag any PTE with no backing VMA as an anomaly.

This is the detection side -- the tool that catches Part 4's stealth injection.

### Background

AArch64 with 4KB granule and 48-bit virtual addresses uses a 4-level page table:

```
PGD [47:39] -> PUD [38:30] -> PMD [29:21] -> PTE [20:12] -> offset [11:0]
```

P4D is folded into PGD on AArch64 48-bit -- `p4d_offset()` is an identity operation, but you must still call it.

At each level, if the entry is empty (`*_none`), skip forward by the level's size. Huge pages (`pud_sect`, `pmd_sect`) -- skip those too.

### What to implement

One TODO block in `scan_ptes()`:

1. **Find the target task:** `rcu_read_lock()` / `find_vpid(pid)` / `pid_task(..., PIDTYPE_PID)` / `get_task_struct(task)` / `rcu_read_unlock()`
2. **Get mm_struct:** `task->mm` (NULL = kernel thread, return `-EINVAL`). Read `mm->task_size`.
3. **Lock and walk:** `mmap_read_lock(mm)`, loop `va` from 0 to `task_size`:
   - `pgd_offset(mm, va)` -- if `pgd_none`, skip `PGDIR_SIZE`
   - `p4d_offset(pgdp, va)` -- if `p4d_none`, skip `P4D_SIZE`
   - `pud_offset(p4dp, va)` -- if `pud_none` or `pud_sect`, skip `PUD_SIZE`
   - `pmd_offset(pudp, va)` -- if `pmd_none` or `pmd_sect`, skip `PMD_SIZE`
   - `pte_offset_kernel(pmdp, va)` -- read with `READ_ONCE`
4. **Check each valid PTE:** if `!pte_none(pte) && pte_valid(pte)`, increment `pte_count`. Call `find_vma(mm, va)` -- if NULL or `vma->vm_start > va`, it's an anomaly.
5. **Clean up:** `mmap_read_unlock(mm)`, `put_task_struct(task)`, log counts with `pr_info`

### Key APIs

```c
pgd_offset / p4d_offset / pud_offset / pmd_offset / pte_offset_kernel
READ_ONCE(*entry)                // Atomic read of page table entry
pgd_none / p4d_none / pud_none / pmd_none / pte_none / pte_valid
pud_sect / pmd_sect              // Huge page checks
find_vma(mm, va)                 // Find VMA covering address
PGDIR_SIZE, P4D_SIZE, PUD_SIZE, PMD_SIZE, PAGE_SIZE
```

---

## Part 4: PTE Stealth Injection -- `map_user_page()` + `do_inject_pte()` (20 pts, challenge)

Inject shellcode by manually constructing page table entries -- no VMA, invisible to `/proc/pid/maps`. Uses a clone trampoline (same pattern as Part 2) for execution.

### Background: The dual-mapping trick

Every physical page has two virtual addresses: the **kernel linear map** (`page_address(page)`) and the **user PTE** you construct. Write through the kernel VA, execute through the user VA.

### What to implement

Three TODO blocks.

**TODO Block 1 (in `map_user_page`):** Walk PGD -> P4D -> PUD -> PMD, allocating intermediate tables as needed, then install the final PTE.

- For each empty intermediate level: `alloc_page(GFP_KERNEL | __GFP_ZERO)`, populate with `WRITE_ONCE()` using the correct type/PXN bits, then `dsb(ishst)` + `isb()`
- Final PTE: `pfn_pte(page_to_pfn(page), PAGE_SHARED_EXEC)` + `set_pte()` + `flush_tlb_all()`

**TODO Block 2 (in `do_inject_pte`):** Allocate pages, write payload, build PTEs.

- `alloc_page` x2 (code + stack), `memcpy` payload via `page_address` (dual-mapping trick)
- `vma_gap_available()` check, then `map_user_page()` x2

**TODO Block 3 (in `do_inject_pte`):** Trampoline + clone hijack (same pattern as Part 2).

### Key APIs

```c
alloc_page(GFP_KERNEL | __GFP_ZERO)   // Allocate zeroed physical page
page_address(page)                      // Kernel linear map VA
pfn_pte(pfn, prot) / PAGE_SHARED_EXEC  // Build user RWX PTE
set_pte / flush_tlb_all                 // Install PTE
WRITE_ONCE / dsb(ishst) / isb()        // Page table barriers
vma_gap_available(mm, addr, len)        // Check for VMA overlap (provided)
```

---

## Part 5: Detect Your Own Injection (10 pts)

Use Part 3's scanner to detect Part 4's injection. After `inject_pte`, run `scan` on the same PID. The scanner should find anomalies at `0x200000000` and `0x200001000`. **No new code needed.**

---

## Grading (100 pts)

| Component | Points |
|-----------|--------|
| Compilation (automatic) | 10 |
| `insmod` + `/dev/kinject` | 5 |
| Part 1: proof file created | 15 |
| Part 2: proof file created | 10 |
| Part 2: target still alive | 5 |
| Part 3: PTE walk count | 10 |
| Part 3: anomaly detection | 10 |
| Part 4: PTE construction | 5 |
| Part 4: proof file created | 10 |
| Part 4: invisible in maps | 10 |
| Part 5: scan detects injection | 10 |
| **Total** | **100** |

## Submit to Gradescope

Upload **`kinject.c`** to Gradescope.

## Debugging Tips

**Check kernel messages.** After `make test-kinject`, look for `kinject:` in `dmesg`.

**Load manually in the VM:**

```bash
insmod /mnt/shared/modules/kinject.ko
sleep 9999 &
echo "inject1 $!" > /dev/kinject         # Part 1
echo "inject2 $!" > /dev/kinject         # Part 2
echo "scan $!" > /dev/kinject            # Part 3
echo "inject_pte $!" > /dev/kinject      # Part 4
echo "scan $!" > /dev/kinject            # Part 5
dmesg | grep kinject
```

**Common pitfalls:**

- Forgetting `READ_ONCE()` when reading page table entries
- Advancing by `PAGE_SIZE` when a higher-level entry was empty (skip by `PGDIR_SIZE` etc.)
- Forgetting `pud_sect`/`pmd_sect` huge page checks in the scan
- Not setting `regs->syscallno = -1` before the PC hijack
- Forgetting `dsb(ishst)` + `isb()` after writing intermediate page table entries
- Forgetting `flush_tlb_all()` after installing the final PTE
- Using `copy_to_user()` for PTE-injected pages (no VMA = kernel rejects it)
- Forgetting `vma_gap_available()` before installing PTEs
- Not calling `mmap_write_lock` before modifying page tables
