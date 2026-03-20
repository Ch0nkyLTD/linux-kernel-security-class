#!/bin/bash
# ==============================================================================
# Test: Kernel Shellcode In-Class Exercises
# ==============================================================================
# Runs inside the QEMU guest via the autotest service.
# Tests offset discovery, raw cred-zeroing, and API-based shellcode.
#
# Expects all files in /mnt/shared/kernel_shellcode/:
#   vuln_rwx.ko, find_offsets.ko, raw_privesc_solution, api_privesc
# ==============================================================================

set -u

SC="/mnt/shared/kernel_shellcode"
RESULTS="/mnt/shared/test_results.log"
PASS=0
FAIL=0

log() { echo "$@" | tee -a "$RESULTS"; }

>"$RESULTS"
log "=== Test: Kernel Shellcode Exercises ==="
log ""

# --- Step 0: Setup ---
log "--- Loading vuln_rwx driver ---"
echo 0 >/proc/sys/kernel/kptr_restrict
insmod "$SC/vuln_rwx.ko" 2>&1 | tee -a "$RESULTS"
if [ -e /dev/vuln_rwx ]; then
  chmod 666 /dev/vuln_rwx
  log "  PASS: /dev/vuln_rwx created"
  PASS=$((PASS + 1))
else
  log "  FAIL: /dev/vuln_rwx not found"
  FAIL=$((FAIL + 1))
  log "=== ABORT: driver failed ==="
  poweroff -f
  exit 1
fi
log ""

# --- Step 1: Discover offsets via find_offsets.ko ---
log "--- Exercise 1: Offset Discovery ---"
dmesg -C
insmod "$SC/find_offsets.ko" 2>/dev/null # returns -EINVAL on purpose

CRED_OFF=$(dmesg | grep 'offsetof(task_struct, cred)' | grep -oP '0x[0-9a-f]+' | head -1)
UID_OFF=$(dmesg | grep 'offsetof(cred, uid)' | grep -oP '0x[0-9a-f]+' | head -1)

if [ -n "$CRED_OFF" ] && [ -n "$UID_OFF" ]; then
  log "  cred offset = $CRED_OFF"
  log "  uid offset  = $UID_OFF"
  log "  PASS: offsets discovered"
  PASS=$((PASS + 1))
else
  log "  FAIL: could not parse offsets from dmesg"
  dmesg | tail -10 | tee -a "$RESULTS"
  FAIL=$((FAIL + 1))
  log "=== ABORT: no offsets ==="
  poweroff -f
  exit 1
fi
log ""

# Create analyst user if not exists
id analyst >/dev/null 2>&1 || useradd -u 1001 analyst 2>/dev/null

# Cache kallsyms for non-root readers
grep -wE 'prepare_kernel_cred|commit_creds|init_task' /proc/kallsyms >/tmp/kallsyms_cache
chmod 644 /tmp/kallsyms_cache

# --- Step 2: Test raw_privesc_solution (direct cred zeroing) ---
log "--- Exercise 2: Raw Cred-Zeroing Shellcode ---"
log "  Running as analyst (uid=1001)..."

RAW_OUT=$(su -s /bin/sh analyst -c "CRED_OFF=$CRED_OFF UID_OFF=$UID_OFF $SC/raw_privesc_solution </dev/null" 2>&1)
RAW_RC=$?
log "$RAW_OUT"

if echo "$RAW_OUT" | grep -q "SUCCESS: uid=0"; then
  log "  PASS: raw privesc achieved root"
  PASS=$((PASS + 1))
else
  log "  FAIL: raw privesc did not achieve root (exit=$RAW_RC)"
  FAIL=$((FAIL + 1))
fi
log ""

# --- Step 3: Test api_privesc (API approach) ---
log "--- Exercise 3: API-Based Shellcode ---"
log "  Running as analyst (uid=1001)..."
API_OUT=$(su -s /bin/sh analyst -c "$SC/api_privesc </dev/null" 2>&1)
API_RC=$?
log "$API_OUT"

if echo "$API_OUT" | grep -q "SUCCESS: uid=0"; then
  log "  PASS: API privesc achieved root"
  PASS=$((PASS + 1))
else
  log "  FAIL: API privesc did not achieve root (exit=$API_RC)"
  FAIL=$((FAIL + 1))
fi
log ""

# --- Summary ---
log "=== Results: $PASS passed, $FAIL failed ==="
if [ $FAIL -eq 0 ]; then
  log "=== ALL TESTS PASSED ==="
else
  log "=== SOME TESTS FAILED ==="
fi

rmmod vuln_rwx 2>/dev/null
poweroff -f
