# ──────────────────────────────────────────────────────────────
# Makefile — MERIDIAN DEFENSE GROUP Vulnerable Service
# ──────────────────────────────────────────────────────────────
#
# Cross-compiles the vulnerable TCP server for AArch64.
# The service runs as "analyst" on port 1337 and has a deliberate
# code-execution vulnerability in the "submit" command.
#
# Targets:
#   make          — build meridian (static aarch64 binary)
#   make install  — build + deploy to QEMU shared folder
#   make setup    — install, boot VM, run setup_capstone.sh
#   make clean    — remove build artifacts

LAB_ROOT ?= $(HOME)/teaching/aarch64-linux-qemu-lab

HOST_ARCH := $(shell uname -m)
ifeq ($(HOST_ARCH),x86_64)
    CC := aarch64-linux-gnu-gcc
else
    CC := gcc
endif

CFLAGS := -Wall -Wextra -O2 -static

.PHONY: all install setup clean

all: meridian

meridian: meridian.c
	$(CC) $(CFLAGS) -o $@ $<

install: all
	@mkdir -p $(LAB_ROOT)/shared/capstone
	cp meridian meridian.service setup_capstone.sh $(LAB_ROOT)/shared/capstone/
	chmod +x $(LAB_ROOT)/shared/capstone/setup_capstone.sh
	@echo ">>> Deployed to shared/capstone/"
	@echo ">>> In guest: sudo bash /mnt/shared/capstone/setup_capstone.sh"

clean:
	rm -f meridian
