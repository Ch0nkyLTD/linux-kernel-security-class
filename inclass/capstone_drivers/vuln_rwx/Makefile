# Makefile for vuln_rwx — vulnerable JIT engine driver
#
# Simulates a buggy JIT compiler that allocates RWX kernel memory
# and executes user-supplied bytecode at EL1. Used for the kernel
# shellcoding exercise (LPE via prepare_kernel_cred/commit_creds).
#
# Usage:
#   make          — build vuln_rwx.ko
#   make install  — copy to QEMU shared folder
#   make clean    — remove build artifacts

obj-m := vuln_rwx.o

ifeq ($(KERNELRELEASE),)

LAB_ROOT ?= $(HOME)/teaching/aarch64-linux-qemu-lab

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

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) modules

install: all
	@mkdir -p $(LAB_ROOT)/shared/modules
	cp vuln_rwx.ko $(LAB_ROOT)/shared/modules/
	@echo ">>> Installed to shared/modules/vuln_rwx.ko"

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) clean 2>/dev/null || true

.PHONY: all install clean

endif
