# ──────────────────────────────────────────────────────────────
# Makefile — C2 Demo In-Class Exercise
# ──────────────────────────────────────────────────────────────
#
# Exercise: Fill TODOs in c2_demo.c to build a covert C2 channel
#           that intercepts kill -62 as rootkit commands.
#
# Targets:
#   make              Build c2_demo.ko (TODO stub version)
#   make solution     Build c2_demo.ko from solutions/
#   make install      Build + deploy to QEMU shared folder
#   make run          Build, deploy, boot VM, run automated test
#   make demo         Build solution, deploy, boot VM interactively
#   make clean        Remove build artifacts

# ── Paths ──

LAB_ROOT ?= $(HOME)/teaching/aarch64-linux-qemu-lab
SHARED   := $(LAB_ROOT)/shared/modules
TIMEOUT  ?= 120

# ── Cross-compile detection ──

HOST_ARCH := $(shell uname -m)
ifeq ($(HOST_ARCH),x86_64)
    KDIR    ?= $(LAB_ROOT)/linux-6.6
    ARCH    := arm64
    CROSS   := aarch64-linux-gnu-
else
    KDIR    ?= /lib/modules/$(shell uname -r)/build
    ARCH    :=
    CROSS   :=
endif

MODULE_NAME := c2_demo
BUILD_DIR   := build
BIN_DIR     := bin

# ── Default: build from student stub ──

.PHONY: all solution install run demo clean

all: $(BIN_DIR)/$(MODULE_NAME).ko

$(BIN_DIR)/$(MODULE_NAME).ko: c2_demo.c
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)
	@cp c2_demo.c $(BUILD_DIR)/
	@echo "obj-m += $(MODULE_NAME).o" > $(BUILD_DIR)/Makefile
	$(MAKE) -C $(KDIR) M=$(CURDIR)/$(BUILD_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) modules
	@cp $(BUILD_DIR)/$(MODULE_NAME).ko $(BIN_DIR)/
	@echo "=== Built: $(BIN_DIR)/$(MODULE_NAME).ko ==="

# ── Build from solution ──

solution:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)
	@cp solutions/c2_demo.c $(BUILD_DIR)/
	@echo "obj-m += $(MODULE_NAME).o" > $(BUILD_DIR)/Makefile
	$(MAKE) -C $(KDIR) M=$(CURDIR)/$(BUILD_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) modules
	@cp $(BUILD_DIR)/$(MODULE_NAME).ko $(BIN_DIR)/
	@echo "=== Built (solution): $(BIN_DIR)/$(MODULE_NAME).ko ==="

# ── Install to QEMU shared folder ──

install: all
	@mkdir -p $(SHARED)
	cp $(BIN_DIR)/$(MODULE_NAME).ko $(SHARED)/
	@echo ">>> Installed to $(SHARED)/$(MODULE_NAME).ko"

# ── Run automated test in QEMU ──

run: install
	@cp $(LAB_ROOT)/tests/test_c2_demo.sh $(LAB_ROOT)/shared/autorun.sh
	@chmod +x $(LAB_ROOT)/shared/autorun.sh
	@echo ">>> Resetting VM and booting (timeout $(TIMEOUT)s)..."
	@rm -f $(LAB_ROOT)/debian-runtime.qcow2
	cd $(LAB_ROOT) && script -qefc "timeout $(TIMEOUT) ./scripts/start.sh --shared --no-debug" shared/boot.log
	@echo ""
	@echo "=== Test output ==="
	@grep -E "PASS|FAIL|=== .* passed" $(LAB_ROOT)/shared/boot.log 2>/dev/null || echo "(check shared/boot.log)"

# ── Demo: build solution, boot VM interactively ──

demo: solution
	@mkdir -p $(SHARED)
	cp $(BIN_DIR)/$(MODULE_NAME).ko $(SHARED)/
	@echo ""
	@echo "=== c2_demo.ko (solution) deployed to shared/modules/ ==="
	@echo ""
	@echo "  Boot the VM:   cd $(LAB_ROOT) && make shared"
	@echo ""
	@echo "  In the VM:"
	@echo "    insmod /mnt/shared/modules/c2_demo.ko"
	@echo "    kill -62 0     # STATUS"
	@echo "    kill -62 1     # toggle process hiding"
	@echo "    kill -62 2     # toggle directory hiding"
	@echo "    kill -62 3     # spawn process from kernel"
	@echo "    dmesg | grep c2_demo"
	@echo "    rmmod c2_demo"
	@echo ""

# ── Clean ──

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
