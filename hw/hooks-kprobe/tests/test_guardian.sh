#!/bin/bash
# ==============================================================================
# Guest-side test: Guardian kernel module (kprobe variant)
# ==============================================================================
# Tests the guardian module that protects /tmp/secret via kprobes:
#   - Bouncer: blocks openat access to /tmp/secret
#   - Cloak: hides "secret" from directory listings
#   - Chardev: magic bytes grant GID 1337 bypass
#
# Output format (parsed by run_tests.py):
#   PASS [pts]: description
#   FAIL [pts]: description
# ==============================================================================

set -u

MODULE="guardian_kprobe"
KO="/mnt/shared/modules/${MODULE}.ko"
DEV="/dev/guardian"
DIR="/tmp/secret"
FILE="$DIR/test.txt"
PASS=0
FAIL=0

pass() { echo "  PASS [$1]: $2"; PASS=$((PASS + 1)); }
fail() { echo "  FAIL [$1]: $2"; FAIL=$((FAIL + 1)); }

echo "=== Test: $MODULE ==="

dmesg -C

# --- Setup: create the protected directory before loading ---
rm -rf "$DIR"
mkdir -p "$DIR"
echo "hidden content" > "$FILE"

# --- Load (10 pts) ---
if insmod "$KO" 2>&1; then
    pass 10 "insmod succeeds"
else
    fail 10 "insmod fails"
    echo "=== $PASS passed, $FAIL failed ==="
    dmesg | tail -20
    exit $FAIL
fi

sleep 0.5

# --- /dev/guardian exists (5 pts) ---
if [ -c "$DEV" ]; then
    pass 5 "/dev/guardian exists as character device"
else
    fail 5 "/dev/guardian not found"
    echo "=== $PASS passed, $FAIL failed ==="
    exit $FAIL
fi

# --- dmesg shows load message (5 pts) ---
if dmesg | grep -qi "guardian.*loaded\|guardian.*init\|guardian.*protecting"; then
    pass 5 "module prints load message"
else
    fail 5 "no load message in dmesg"
fi

# --- ls /tmp/ hides "secret" (15 pts) ---
if ls /tmp/ | grep -q "^secret$"; then
    fail 15 "/tmp/secret visible in listing (cloak not working)"
else
    pass 15 "/tmp/secret hidden from ls"
fi

# --- cat /tmp/secret/test.txt denied (15 pts) ---
if cat "$FILE" 2>/dev/null; then
    fail 15 "$FILE accessible (bouncer not working)"
else
    pass 15 "$FILE access denied"
fi

# --- Magic bytes grant GID (10 pts) ---
# Write 0xDEADBEEF (little-endian) to /dev/guardian
printf '\xef\xbe\xad\xde' > "$DEV" 2>/dev/null
if id -G | tr ' ' '\n' | grep -q "^1337$"; then
    pass 10 "magic bytes granted GID 1337"
else
    # Check via dmesg as fallback
    if dmesg | grep -q "granted GID 1337"; then
        pass 10 "magic bytes granted GID 1337 (confirmed via dmesg)"
    else
        fail 10 "magic bytes did not grant GID 1337"
    fi
fi

# --- ls /tmp/ shows "secret" after bypass (10 pts) ---
if ls /tmp/ | grep -q "^secret$"; then
    pass 10 "/tmp/secret visible after magic GID bypass"
else
    fail 10 "/tmp/secret still hidden after magic GID"
fi

# --- cat /tmp/secret/test.txt after bypass (10 pts) ---
if cat "$FILE" > /dev/null 2>&1; then
    pass 10 "$FILE accessible after magic GID bypass"
else
    fail 10 "$FILE still denied after magic GID"
fi

# --- cat /dev/guardian shows events (5 pts) ---
EVENTS=$(cat "$DEV" 2>/dev/null)
if [ -n "$EVENTS" ]; then
    pass 5 "/dev/guardian returns event log entries"
else
    fail 5 "/dev/guardian returned no events"
fi

# --- Unload (5 pts) ---
dmesg -C
if rmmod "$MODULE" 2>&1; then
    pass 5 "rmmod succeeds"
else
    fail 5 "rmmod fails"
fi

sleep 0.3

# --- /dev/guardian removed (0 pts — freebie check) ---
if [ ! -e "$DEV" ]; then
    pass 0 "/dev/guardian removed after rmmod"
else
    fail 0 "/dev/guardian still exists after rmmod"
fi

# --- dmesg shows unload message (0 pts — freebie check) ---
if dmesg | grep -qi "guardian.*unloaded\|guardian.*exit"; then
    pass 0 "module prints unload message"
else
    fail 0 "no unload message in dmesg"
fi

# --- Cleanup ---
rm -rf "$DIR"

echo ""
echo "=== $PASS passed, $FAIL failed ==="
echo ""
echo "=== dmesg ==="
dmesg | tail -20

exit $FAIL
