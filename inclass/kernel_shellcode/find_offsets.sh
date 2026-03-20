#!/bin/bash
# find_offsets.sh - Extract struct offsets from vmlinux using pahole/nm
#
# Usage: ./find_offsets.sh [path-to-vmlinux]
#
# If no vmlinux path given, tries common locations.

set -e

VMLINUX="${1:-}"

if [ -z "$VMLINUX" ]; then
    for candidate in \
        /boot/vmlinux-$(uname -r) \
        /lib/modules/$(uname -r)/build/vmlinux \
        /usr/lib/debug/boot/vmlinux-$(uname -r) \
        ./vmlinux; do
        if [ -f "$candidate" ]; then
            VMLINUX="$candidate"
            break
        fi
    done
fi

if [ -z "$VMLINUX" ] || [ ! -f "$VMLINUX" ]; then
    echo "[-] Cannot find vmlinux. Pass the path as an argument."
    echo "    Usage: $0 /path/to/vmlinux"
    exit 1
fi

echo "[*] Using vmlinux: $VMLINUX"
echo

echo "=== task_struct -> cred offset ==="
pahole -C task_struct "$VMLINUX" | grep -w 'cred' | head -3
echo

echo "=== cred field offsets ==="
pahole -C cred "$VMLINUX" | grep -E '(uid|gid|usage)' | head -10
echo

echo "=== Kernel symbol addresses (for API approach) ==="
aarch64-linux-gnu-nm "$VMLINUX" 2>/dev/null | grep -E '(prepare_kernel_cred|commit_creds)$' || \
    nm "$VMLINUX" 2>/dev/null | grep -E '(prepare_kernel_cred|commit_creds)$' || \
    echo "(nm not available or symbols stripped)"
