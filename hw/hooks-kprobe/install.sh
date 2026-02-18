#!/usr/bin/env bash
# Install this assignment into your aarch64-linux-qemu-lab
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LAB_DIR="${1:-$SCRIPT_DIR/../aarch64-linux-qemu-lab}"
MODULE="guardian_kprobe"

# --- Validate lab directory ---
if [ ! -f "$LAB_DIR/modules/module.mk" ]; then
    echo "ERROR: Lab repo not found at $LAB_DIR"
    echo ""
    echo "Usage: ./install.sh [path/to/aarch64-linux-qemu-lab]"
    echo ""
    echo "If the lab is not next to this directory, pass its path:"
    echo "  ./install.sh ~/path/to/aarch64-linux-qemu-lab"
    exit 1
fi

LAB_DIR="$(cd "$LAB_DIR" && pwd)"

echo "[*] Installing Guardian — Kprobe assignment into lab..."
echo "    Lab: $LAB_DIR"
echo ""

# --- Copy module directory ---
if [ -d "$LAB_DIR/modules/$MODULE" ]; then
    echo "    modules/$MODULE/ already exists — overwriting source files"
fi
mkdir -p "$LAB_DIR/modules/$MODULE"
cp "$SCRIPT_DIR/$MODULE"/*.c "$LAB_DIR/modules/$MODULE/" 2>/dev/null || true
cp "$SCRIPT_DIR/$MODULE"/*.h "$LAB_DIR/modules/$MODULE/" 2>/dev/null || true
cp "$SCRIPT_DIR/$MODULE/Makefile" "$LAB_DIR/modules/$MODULE/"
echo "    Copied: modules/$MODULE/"

# --- Copy test script ---
mkdir -p "$LAB_DIR/tests"
cp "$SCRIPT_DIR/tests/"*.sh "$LAB_DIR/tests/"
chmod +x "$LAB_DIR/tests/"*.sh
echo "    Copied: tests/test_guardian.sh"

echo ""
echo "[*] Done! Next steps:"
echo ""
echo "    cd $LAB_DIR/modules/$MODULE"
echo "    make                         # Build the module"
echo "    cd $LAB_DIR"
echo "    make test-$MODULE            # Build, boot VM, run tests"
echo ""
echo "    When ready to submit:"
echo "    cd $LAB_DIR/modules/$MODULE"
echo "    make submission.zip          # Creates zip for Gradescope"
