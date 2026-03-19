#!/usr/bin/env bash
# ==============================================================================
# Test: Reflective Memfd Payload
# ==============================================================================
# Runs inside the QEMU VM. Executes the test harness with the mempayload
# and checks each stage of the reflective loading pipeline.
#
# Output format: PASS/FAIL [pts]: description
# Results written to /mnt/shared/test_results.log
# ==============================================================================

set -uo pipefail

SHARED="/mnt/shared"
RESULTS="$SHARED/test_results.log"
PAYLOAD="$SHARED/mempayload"
HARNESS="$SHARED/test_harness"

# Mount shared folder if not already mounted
if ! mountpoint -q "$SHARED" 2>/dev/null; then
    mkdir -p "$SHARED"
    mount -t 9p -o trans=virtio hostshare "$SHARED" -oversion=9p2000.L 2>/dev/null || true
fi

exec > "$RESULTS" 2>&1

FAIL_COUNT=0

pass() { echo "  PASS [$1]: $2"; }
fail() { echo "  FAIL [$1]: $2"; FAIL_COUNT=$((FAIL_COUNT + 1)); }

echo "=== Reflective Memfd Payload Tests ==="
echo ""

# --- Bootstrap assembled (10 pts) ---
# The mempayload should be larger than 4096 bytes (bootstrap stub + SO)
if [ -f "$PAYLOAD" ]; then
    SIZE=$(wc -c < "$PAYLOAD")
    if [ "$SIZE" -gt 4096 ]; then
        pass 10 "Bootstrap assembled (mempayload = ${SIZE} bytes)"
    else
        fail 10 "mempayload too small (${SIZE} bytes, expected > 4096)"
    fi
else
    fail 10 "mempayload not found"
fi

# --- Run the harness and capture output ---
# Clean up any previous proof files
rm -f /tmp/payload_loaded_*

OUTPUT=""
if [ -f "$HARNESS" ] && [ -f "$PAYLOAD" ]; then
    chmod +x "$HARNESS"
    OUTPUT=$("$HARNESS" "$PAYLOAD" 2>&1 || true)
    echo ""
    echo "--- Harness output ---"
    echo "$OUTPUT"
    echo "--- End harness output ---"
    echo ""
fi

# --- find_base() (15 pts) ---
if echo "$OUTPUT" | grep -q "found ELF base"; then
    pass 15 "find_base() locates ELF header"
else
    fail 15 "find_base() did not find ELF base (missing 'found ELF base' in output)"
fi

# --- search_lib_procmaps() (15 pts) ---
if echo "$OUTPUT" | grep -q "found libc"; then
    pass 15 "search_lib_procmaps() finds libc"
else
    fail 15 "search_lib_procmaps() did not find libc (missing 'found libc' in output)"
fi

# --- find_symbol_symtab() (15 pts) ---
# If dlopen was resolved, the loader prints "loading via /proc/self/fd/..."
if echo "$OUTPUT" | grep -q "loading via /proc/self/fd/"; then
    pass 15 "find_symbol_symtab() resolves dlopen/sprintf"
else
    fail 15 "find_symbol_symtab() failed (missing 'loading via /proc/self/fd/' in output)"
fi

# --- memfd_create (10 pts) ---
if echo "$OUTPUT" | grep -q "memfd_create failed"; then
    fail 10 "memfd_create syscall failed"
else
    if echo "$OUTPUT" | grep -q "loading via /proc/self/fd/"; then
        pass 10 "memfd_create succeeded"
    elif echo "$OUTPUT" | grep -q "found ELF base"; then
        # Got past find_base but no memfd error — likely memfd worked
        # but something else failed later
        pass 10 "memfd_create succeeded"
    else
        fail 10 "Cannot determine memfd_create status (loader may not have reached this point)"
    fi
fi

# --- dlopen via memfd (15 pts) ---
if echo "$OUTPUT" | grep -q "\[bootstrap\] done"; then
    pass 15 "dlopen via memfd succeeded"
else
    fail 15 "dlopen via memfd failed (missing '[bootstrap] done' in output)"
fi

# --- Constructor proof file (10 pts) ---
PROOF_FILE=$(ls /tmp/payload_loaded_* 2>/dev/null | head -1)
if [ -n "$PROOF_FILE" ]; then
    CONTENT=$(cat "$PROOF_FILE" 2>/dev/null)
    if echo "$CONTENT" | grep -q "PAYLOAD_OK"; then
        pass 10 "Constructor created proof file with PAYLOAD_OK"
    else
        fail 10 "Proof file exists but does not contain PAYLOAD_OK (got: $CONTENT)"
    fi
else
    fail 10 "Constructor proof file /tmp/payload_loaded_<pid> not found"
fi

echo ""
echo "=== Summary: $FAIL_COUNT failure(s) ==="

# Power off the VM
poweroff -f 2>/dev/null || true
