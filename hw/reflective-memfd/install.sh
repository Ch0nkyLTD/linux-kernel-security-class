#!/usr/bin/env bash
# Install this assignment into your aarch64-linux-qemu-lab
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LAB_DIR="${1:-$SCRIPT_DIR/../aarch64-linux-qemu-lab}"

# --- Validate lab directory ---
if [ ! -d "$LAB_DIR/shared" ] && [ ! -f "$LAB_DIR/scripts/start.sh" ]; then
    echo "ERROR: Lab repo not found at $LAB_DIR"
    echo ""
    echo "Usage: ./install.sh [path/to/aarch64-linux-qemu-lab]"
    echo ""
    echo "If the lab is not next to this directory, pass its path:"
    echo "  ./install.sh ~/path/to/aarch64-linux-qemu-lab"
    exit 1
fi

LAB_DIR="$(cd "$LAB_DIR" && pwd)"

echo "[*] Installing Reflective Memfd Payload assignment into lab..."
echo "    Lab: $LAB_DIR"
echo ""

# --- Copy source files to shared/ for building ---
mkdir -p "$LAB_DIR/shared/reflective-memfd"
cp "$SCRIPT_DIR/reflective-memfd"/* "$LAB_DIR/shared/reflective-memfd/"
echo "    Copied: shared/reflective-memfd/"

# --- Copy test script ---
mkdir -p "$LAB_DIR/tests"
cp "$SCRIPT_DIR/tests/"*.sh "$LAB_DIR/tests/"
chmod +x "$LAB_DIR/tests/"*.sh
echo "    Copied: tests/test_reflective.sh"

echo ""
echo "[*] Done! Next steps:"
echo ""
echo "    cd $LAB_DIR/shared/reflective-memfd"
echo "    make                         # Cross-compile everything"
echo ""
echo "    # Then from the lab root:"
echo "    cp shared/reflective-memfd/mempayload shared/"
echo "    cp shared/reflective-memfd/test_harness shared/"
echo "    cp tests/test_reflective.sh shared/autorun.sh"
echo "    chmod +x shared/autorun.sh"
echo "    rm -f debian-runtime.qcow2   # force fresh VM"
echo "    make nodebug                  # boot + test"
echo ""
echo "    Submit loader.c, procfsutil.h, and elfsym.h to Gradescope."
