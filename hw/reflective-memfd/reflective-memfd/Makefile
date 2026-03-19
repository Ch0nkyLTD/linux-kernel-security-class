# ==============================================================================
# Reflective Memfd Payload — Build Rules
# ==============================================================================
# Cross-compiles the reflective SO, generates bootstrap stub, and
# concatenates into the final mempayload binary.
#
# Usage:
#   make                  Build everything (mempayload + test_harness)
#   make clean            Remove build artifacts
#
# Requirements:
#   - aarch64-linux-gnu-gcc (cross-compiler)
#   - python3 (for bootstrap.py)
# ==============================================================================

CROSS   := aarch64-linux-gnu-
CC      := $(CROSS)gcc
AS      := $(CROSS)as
OBJCOPY := $(CROSS)objcopy
HOSTCC  := gcc

# Reflective SO flags: no libc, position-independent, shared object
SO_CFLAGS := -nostdlib -fPIC -shared -ffreestanding \
             -Wall -Wno-builtin-declaration-mismatch \
             -e load_memfd -o rp.so

# Test harness: native aarch64 binary (linked with libc)
HARNESS_CFLAGS := -Wall -o test_harness

.PHONY: all clean

all: mempayload test_harness

# --- Reflective shared object ---
rp.so: loader.c payload.c procfsutil.h elfsym.h syscall.h minstr.h
	$(CC) $(SO_CFLAGS) loader.c payload.c

# --- Bootstrap stub ---
bootstrap.S: rp.so bootstrap.py
	python3 bootstrap.py rp.so > bootstrap.S

bootstrap.o: bootstrap.S
	$(AS) bootstrap.S -o bootstrap.o

bootstrap.bin: bootstrap.o
	$(OBJCOPY) -O binary -j .text bootstrap.o bootstrap.bin

# --- Final payload: bootstrap + SO concatenated ---
mempayload: bootstrap.bin rp.so
	cat bootstrap.bin rp.so > mempayload
	@echo "[+] Built mempayload: $$(wc -c < mempayload) bytes"

# --- Test harness (runs on target, needs libc) ---
test_harness: test_harness.c
	$(CC) $(HARNESS_CFLAGS) test_harness.c

clean:
	rm -f rp.so bootstrap.S bootstrap.o bootstrap.bin mempayload test_harness
