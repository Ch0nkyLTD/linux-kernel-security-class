# ──────────────────────────────────────────────────────────────
# Makefile — Kernel Shellcode In-Class Exercises
# ──────────────────────────────────────────────────────────────
#
# Exercise 1: insmod find_offsets.ko → read dmesg → fill offsets.h
# Exercise 2: Fill TODOs in raw_privesc.c → ./raw_privesc
# Exercise 3: Run api_privesc alongside raw_privesc → compare
#
# Targets:
#   make all            Build userland exploits (default)
#   make module         Build kernel modules (find_offsets + vuln_rwx)
#   make install        Build everything, deploy to QEMU shared folder
#   make run            Build, deploy, boot VM, run tests end-to-end
#   make clean          Remove build artifacts

# ── Paths ──

LAB_ROOT ?= $(HOME)/teaching/aarch64-linux-qemu-lab
SHARED   := $(LAB_ROOT)/shared/kernel_shellcode
TIMEOUT  ?= 120

# ── Cross-compile detection ──

HOST_ARCH := $(shell uname -m)
ifeq ($(HOST_ARCH),x86_64)
    CC      := aarch64-linux-gnu-gcc
    KDIR    ?= $(LAB_ROOT)/linux-6.6
    ARCH    := arm64
    CROSS   := aarch64-linux-gnu-
else
    CC      := gcc
    KDIR    ?= /lib/modules/$(shell uname -r)/build
    ARCH    :=
    CROSS   :=
endif

CFLAGS := -Wall -Wextra -O2

# ── Default target: userland exploits ──

.PHONY: all module install run clean

all: raw_privesc raw_privesc_solution api_privesc

raw_privesc: raw_privesc.c offsets.h
	$(CC) $(CFLAGS) -o $@ $<

raw_privesc_solution: raw_privesc_solution.c offsets.h
	$(CC) $(CFLAGS) -o $@ $<

api_privesc: api_privesc.c
	$(CC) $(CFLAGS) -o $@ $<

# ── Kernel modules ──

obj-m := find_offsets.o vuln_rwx.o

module: find_offsets.c vuln_rwx.c
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) modules

# ── Install to QEMU shared folder ──

install: all module
	@mkdir -p $(SHARED)
	cp vuln_rwx.ko find_offsets.ko $(SHARED)/
	cp raw_privesc_solution api_privesc $(SHARED)/
	cp autorun.sh $(LAB_ROOT)/shared/autorun.sh
	chmod +x $(LAB_ROOT)/shared/autorun.sh
	@echo ">>> Deployed to $(SHARED)"
	@echo ">>> autorun.sh installed at $(LAB_ROOT)/shared/autorun.sh"

# ── Run end-to-end test in QEMU ──

run: install
	@echo ">>> Resetting VM and booting (timeout $(TIMEOUT)s)..."
	rm -f $(LAB_ROOT)/debian-runtime.qcow2
	cd $(LAB_ROOT) && script -qefc "timeout $(TIMEOUT) ./scripts/start.sh --shared --no-debug" shared/boot.log
	@echo ""
	@if [ -f $(LAB_ROOT)/shared/test_results.log ]; then \
		echo "=== Test Results ==="; \
		cat $(LAB_ROOT)/shared/test_results.log; \
		rm -f $(LAB_ROOT)/shared/autorun.sh; \
	fi

# ── Clean ──

clean:
	rm -f raw_privesc raw_privesc_solution api_privesc
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) clean 2>/dev/null || true
