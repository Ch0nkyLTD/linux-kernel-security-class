#!/usr/bin/env python3
"""
Generate an AArch64 bootstrap stub for a reflective SO.

The stub:
  1. Loads the total payload size into x0
  2. Branches forward over a NOP sled into the SO's entry point

Usage:
  python3 bootstrap.py rp.so > bootstrap.S

Then:
  aarch64-linux-gnu-as bootstrap.S -o bootstrap.o
  aarch64-linux-gnu-objcopy -O binary -j .text bootstrap.o bootstrap.bin
  cat bootstrap.bin rp.so > mempayload
"""
import argparse
import math
import os
import subprocess
import sys


def get_entry_offset(elf_path):
    """Extract the ELF entry point address via readelf."""
    cmd = f"readelf -h {elf_path} | grep 'Entry point address:' | awk '{{print $4}}'"
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True, check=True)
    offset_str = result.stdout.strip()
    if not offset_str.startswith("0x"):
        print(f"[-] Unexpected readelf output: '{offset_str}'", file=sys.stderr)
        sys.exit(1)
    offset = int(offset_str, 16)
    print(f"[+] Entry point offset: {offset:#x}", file=sys.stderr)
    return offset


def generate_bootstrap(elf_size, entry_offset, n_instructions=2):
    """
    Generate AArch64 assembly for the bootstrap stub.

    Layout in memory:
      [bootstrap instructions] [NOP sled to 4KB] [SO bytes...]
      ^-- PC starts here       ^-- padding        ^-- entry_offset relative to SO start
    """
    # Round instruction block up to 16-byte alignment
    block_size = int(math.ceil(n_instructions / 16) * 16)

    # Fill remaining space to 4KB with NOPs
    nop_count = (4096 - block_size) // 4
    nop_size = 4 * nop_count
    nops = "    nop\n" * nop_count

    # Branch target: skip over the rest of the stub + NOP sled,
    # then land at the entry point within the SO
    branch_target = entry_offset + block_size + nop_size - 4

    asm = f"""\
    .arch armv8-a
    .text
    .global _start

_start:
    ldr x0, ={elf_size}          // x0 = total payload size (arg to load_memfd)
    b . + {branch_target:#x}     // branch into SO at entry point
{nops}"""

    return asm


def main():
    parser = argparse.ArgumentParser(
        description="Generate AArch64 bootstrap assembly for a reflective SO.")
    parser.add_argument("elf_file", help="Path to the compiled reflective SO (rp.so)")
    args = parser.parse_args()

    elf_path = args.elf_file
    if not os.path.exists(elf_path):
        print(f"[-] File not found: {elf_path}", file=sys.stderr)
        sys.exit(1)

    with open(elf_path, "rb") as f:
        elf_size = len(f.read())
    print(f"[*] SO size: {elf_size} bytes", file=sys.stderr)

    entry_offset = get_entry_offset(elf_path)
    bootstrap_asm = generate_bootstrap(elf_size, entry_offset)
    print(bootstrap_asm)


if __name__ == "__main__":
    main()
