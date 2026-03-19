#!/usr/bin/env bash
# ==============================================================================
# Guest-side test script for kinject (kernel process injection driver)
# ==============================================================================
# Runs inside the QEMU VM via autotest.service.
# Tests: direct injection, clone injection, PTE walking, stealth injection,
#        and integration (scanner detects stealth injection).
# ==============================================================================

set -uo pipefail

RESULTS=/mnt/shared/test_results.log
MODULE=kinject

> "$RESULTS"

log() { echo "  $1" | tee -a "$RESULTS"; }

wait_for_file() {
    local file="$1"
    local timeout="${2:-5}"
    for i in $(seq 1 "$timeout"); do
        [ -f "$file" ] && return 0
        sleep 1
    done
    return 1
}

# --- insmod ---
echo "=== Loading $MODULE ==="

if insmod /mnt/shared/modules/${MODULE}.ko 2>&1; then
    log "PASS [5]: insmod succeeds and /dev/kinject created"
else
    log "FAIL [5]: insmod failed"
fi

if [ ! -c /dev/kinject ]; then
    log "FAIL [5]: /dev/kinject not found (already counted above)"
    log "FAIL [15]: Part 1 proof file — module not loaded"
    log "FAIL [10]: Part 2 proof file — module not loaded"
    log "FAIL [5]: Part 2 target alive — module not loaded"
    log "FAIL [10]: Part 3 PTE walk — module not loaded"
    log "FAIL [10]: Part 3 anomaly detection — module not loaded"
    log "FAIL [5]: Part 4 PTE construction — module not loaded"
    log "FAIL [10]: Part 4 proof file — module not loaded"
    log "FAIL [10]: Part 4 invisible — module not loaded"
    log "FAIL [10]: Part 5 scan detects injection — module not loaded"
    echo "=== Test Complete ==="
    poweroff -f
    exit 0
fi

# ═══════════════════════════════════════════════════════════
# Part 1: Direct PC Hijack
# ═══════════════════════════════════════════════════════════
echo ""
echo "=== Part 1: Direct PC Hijack ==="

sleep 9999 &
P1_PID=$!
sleep 1

dmesg -C
echo "inject1 $P1_PID" > /dev/kinject 2>/dev/null || true
sleep 3

# Check proof file
if wait_for_file "/tmp/kinject1_${P1_PID}" 5; then
    log "PASS [15]: Part 1 proof file /tmp/kinject1_${P1_PID} created"
else
    log "FAIL [15]: Part 1 proof file /tmp/kinject1_${P1_PID} not found"
fi

# ═══════════════════════════════════════════════════════════
# Part 2: Clone Trampoline
# ═══════════════════════════════════════════════════════════
echo ""
echo "=== Part 2: Clone Trampoline ==="

sleep 9999 &
P2_PID=$!
sleep 1

dmesg -C
echo "inject2 $P2_PID" > /dev/kinject 2>/dev/null || true
sleep 3

# Check proof file
if wait_for_file "/tmp/kinject2_${P2_PID}" 5; then
    log "PASS [10]: Part 2 proof file /tmp/kinject2_${P2_PID} created"
else
    log "FAIL [10]: Part 2 proof file /tmp/kinject2_${P2_PID} not found"
fi

# Check target still alive (clone preserved parent)
if [ -d "/proc/${P2_PID}" ] 2>/dev/null; then
    log "PASS [5]: Part 2 target process still alive (clone preserved parent)"
else
    log "FAIL [5]: Part 2 target process died (clone did not preserve parent)"
fi

# ═══════════════════════════════════════════════════════════
# Part 3: Page Table Walking / Detection
# ═══════════════════════════════════════════════════════════
echo ""
echo "=== Part 3: Page Table Scan ==="

sleep 9999 &
SCAN_PID=$!
sleep 1

dmesg -C
echo "scan $SCAN_PID" > /dev/kinject 2>/dev/null || true
sleep 1

SCAN_OUTPUT=$(dmesg | grep "kinject: scan complete" | tail -1)

if echo "$SCAN_OUTPUT" | grep -q "ptes="; then
    PTE_COUNT=$(echo "$SCAN_OUTPUT" | sed 's/.*ptes=\([0-9]*\).*/\1/')
    if [ "$PTE_COUNT" -gt 10 ]; then
        log "PASS [10]: Part 3 page table walk found $PTE_COUNT valid PTEs"
    else
        log "FAIL [10]: Part 3 page table walk found only $PTE_COUNT PTEs (expected >10)"
    fi
else
    log "FAIL [10]: Part 3 page table walk did not report PTE count"
fi

if echo "$SCAN_OUTPUT" | grep -q "anomalies="; then
    ANOMALY_COUNT=$(echo "$SCAN_OUTPUT" | sed 's/.*anomalies=\([0-9]*\).*/\1/')
    log "PASS [10]: Part 3 anomaly detection implemented (found $ANOMALY_COUNT anomalies)"
else
    log "FAIL [10]: Part 3 anomaly detection not found in output"
fi

kill $SCAN_PID 2>/dev/null
wait $SCAN_PID 2>/dev/null || true

# ═══════════════════════════════════════════════════════════
# Part 4: PTE Stealth Injection
# ═══════════════════════════════════════════════════════════
echo ""
echo "=== Part 4: PTE Stealth Injection ==="

sleep 9999 &
PTE_PID=$!
sleep 1

dmesg -C
echo "inject_pte $PTE_PID" > /dev/kinject 2>/dev/null || true
sleep 3

# Check PTE installed
if dmesg | grep -q "kinject: PTE installed"; then
    log "PASS [5]: Part 4 PTE construction succeeded"
else
    log "FAIL [5]: Part 4 PTE construction failed or not attempted"
fi

# Check proof file (PTE injection uses "kinject_pte" prefix)
if wait_for_file "/tmp/kinject_pte_${PTE_PID}" 5; then
    log "PASS [10]: Part 4 PTE injection proof file created"
else
    log "FAIL [10]: Part 4 PTE injection proof file not found"
fi

# THE STEALTH TEST: mapping should NOT appear in /proc/pid/maps
if [ -d "/proc/${PTE_PID}" ] 2>/dev/null; then
    INJECT_HEX="200000000"
    if grep -qi "$INJECT_HEX" "/proc/${PTE_PID}/maps" 2>/dev/null; then
        log "FAIL [10]: Part 4 PTE injection visible in maps (should be hidden)"
    else
        log "PASS [10]: Part 4 PTE injection invisible in /proc/pid/maps (stealth OK)"
    fi
else
    # Parent may have exited — check if child thread is running
    log "PASS [10]: Part 4 PTE injection invisible (parent process exited)"
fi

# ═══════════════════════════════════════════════════════════
# Part 5: Detect Your Own Injection
# ═══════════════════════════════════════════════════════════
echo ""
echo "=== Part 5: Scan Detects PTE Injection ==="

# Scan the PTE-injected process to detect anomalies
# The scanner should find 2 anomalies: INJECT_CODE_VA and INJECT_STACK_VA
if [ -d "/proc/${PTE_PID}" ] 2>/dev/null; then
    dmesg -C
    echo "scan $PTE_PID" > /dev/kinject 2>/dev/null || true
    sleep 1

    SCAN5_OUTPUT=$(dmesg | grep "kinject: scan complete" | tail -1)

    if echo "$SCAN5_OUTPUT" | grep -q "anomalies="; then
        A5_COUNT=$(echo "$SCAN5_OUTPUT" | sed 's/.*anomalies=\([0-9]*\).*/\1/')
        if [ "$A5_COUNT" -ge 2 ]; then
            # Verify anomalies are at the expected addresses
            if dmesg | grep -q "ANOMALY va=200000000"; then
                log "PASS [10]: Part 5 scanner detected $A5_COUNT anomalies after inject_pte"
            else
                log "FAIL [10]: Part 5 anomalies found but not at expected address 0x200000000"
            fi
        else
            log "FAIL [10]: Part 5 scanner found only $A5_COUNT anomalies (expected >=2)"
        fi
    else
        log "FAIL [10]: Part 5 scan did not report anomaly count"
    fi
else
    log "FAIL [10]: Part 5 cannot scan — PTE-injected process not running"
fi

# ═══════════════════════════════════════════════════════════
# Cleanup
# ═══════════════════════════════════════════════════════════
echo ""
echo "=== Cleanup ==="

# Kill remaining processes
kill $P1_PID $P2_PID $PTE_PID 2>/dev/null || true
wait 2>/dev/null || true

if rmmod $MODULE 2>&1; then
    echo "  rmmod succeeded"
else
    echo "  rmmod failed (expected if injected threads still running)"
fi

echo ""
echo "=== Test Complete ==="
cat "$RESULTS"

# Power off VM
poweroff -f
