# Makefile for vuln_rw — vulnerable debug interface driver
#
# Simulates a debug interface left in production that exposes two
# IOCTLs for arbitrary kernel read/write with no address validation.
# Used for the capstone privilege escalation challenges.
#
# Usage:
#   make          — build vuln_rw.ko
#   make install  — copy to QEMU shared folder
#   make clean    — remove build artifacts

obj-m := vuln_rw.o

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
	cp vuln_rw.ko $(LAB_ROOT)/shared/modules/
	@echo ">>> Installed to shared/modules/vuln_rw.ko"

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) clean 2>/dev/null || true

.PHONY: all install clean

endif
