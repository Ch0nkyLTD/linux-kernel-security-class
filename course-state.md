# Course State — CY-4973 Linux Kernel Security

## Current Position
- **Last lecture:** reflective-payloads (2026-03-13)
- **Next planned:** TBD
- **Lectures delivered:** 13 (excluding empty 01-27)
- **Total slides delivered:** ~715

## Lecture History

| Date | Topic File | Slides | Concepts Introduced | Exercises |
|------|-----------|--------|---------------------|-----------|
| 01-13 | armv8-a_crash_course, aarch64_asm_snippets | 34 | AArch64 registers, EL0/EL1, adr/ldr, stp/ldp | — |
| 01-16 | linux-basics, basic_elf, ko-basics | 82 | syscalls, ELF format, insmod, chardev, module_init | — |
| 01-20 | compiler-linking, linux_file_io | 25 | dynamic linking, GOT/PLT, open/read/write | — |
| 01-30 | linux_filesystem_ops, elf-in-depth-and-process-creation | 75 | VFS, inodes, ELF sections, execve, fork | — |
| 02-03 | ld_preload_0 | 45 | LD_PRELOAD, symbol interposition, dlsym | — |
| 02-06 | linux-kernel-basics | 45 | task_struct, cdev, file_operations, ioctl | — |
| 02-10 | promote, kprobes | 109 | cred struct, prepare_creds, kprobe, kretprobe, pt_regs | — |
| 02-13 | ftrace | 61 | ftrace_ops, patchable function entry, nop-to-bl | — |
| 02-17 | kallsyms, syscall-table | 49 | kallsyms_lookup_name, sys_call_table, PTE write-protect | — |
| 02-20 | virtual-memory-deepdive | 54 | 4-level page tables, TTBR0/TTBR1, TLB, PTE format | — |
| 02-24 | capstone-kernel | 40 | capstone chain, vuln drivers, rootkit architecture | — |
| 02-27 | kernel-code-injection | 61 | mm_struct, VMA, PTE construction, kernel_clone | — |
| 03-13 | reflective-payloads | 35 | memfd_create, /dev/shm, dlopen, reflective SO | — |

## Unscheduled Topic Files

These lectures exist as topic files but are not yet assigned to schedule dates:
- kernel-shellcoding.md (22 slides)
- kernel-contexts.md
- process-hiding.md
- kthreads.md
- shellcode-loaders.md
- capstone-environment.md
- full-chain-rootkit.md

## Tag Index

| Tag | Lectures |
|-----|----------|
| aarch64 | armv8-a_crash_course, aarch64_asm_snippets, kernel-shellcoding |
| elf | basic_elf, elf-in-depth-and-process-creation |
| syscalls | linux-basics, linux_file_io, linux_filesystem_ops |
| linking | compiler-linking, ld_preload_0 |
| kernel-basics | ko-basics, linux-kernel-basics |
| hooking | kprobes, ftrace, syscall-table, kallsyms |
| credentials | promote, kernel-shellcoding |
| virtual-memory | virtual-memory-deepdive, syscall-table |
| injection | kernel-code-injection, reflective-payloads |
| capstone | capstone-kernel, capstone-environment, full-chain-rootkit |
| process-management | process-hiding, kernel-contexts |
| stealth | process-hiding, reflective-payloads, shellcode-loaders |
| kthreads | kthreads |

## Knowledge Stats
- **Functions:** 55 registered
- **Types/Structs:** 18 registered
- **Concepts:** 24 registered
- **Syscalls:** 14 registered
- **Tools/Techniques:** 8 registered
- **Total:** 119 entries
- **Last updated:** 2026-03-17
