#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LAB_DIR="${1:-$SCRIPT_DIR/../aarch64-linux-qemu-lab}"
MODULE="kinject"

if [ ! -f "$LAB_DIR/modules/module.mk" ]; then
    echo "ERROR: Lab repo not found at $LAB_DIR"
    echo "Usage: ./install.sh [path/to/aarch64-linux-qemu-lab]"
    exit 1
fi

LAB_DIR="$(cd "$LAB_DIR" && pwd)"

echo "[*] Installing Kernel Process Injection assignment into lab..."
echo "    Lab: $LAB_DIR"

mkdir -p "$LAB_DIR/modules/$MODULE"
cp "$SCRIPT_DIR/$MODULE"/*.c "$LAB_DIR/modules/$MODULE/" 2>/dev/null || true
cp "$SCRIPT_DIR/$MODULE"/*.h "$LAB_DIR/modules/$MODULE/" 2>/dev/null || true
cp "$SCRIPT_DIR/$MODULE/Makefile" "$LAB_DIR/modules/$MODULE/"
echo "    Copied: modules/$MODULE/"

mkdir -p "$LAB_DIR/tests"
cp "$SCRIPT_DIR/tests/"*.sh "$LAB_DIR/tests/"
chmod +x "$LAB_DIR/tests/"*.sh
echo "    Copied: tests/test_kinject.sh"

echo ""
echo "[*] Done! Next steps:"
echo "    cd $LAB_DIR"
echo "    make module-$MODULE"
echo "    make test-$MODULE"
echo ""
echo "    Submit kinject.c to Gradescope when ready."
