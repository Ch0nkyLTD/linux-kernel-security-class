CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -std=c99
PICFLAGS ?= -fPIC
LDSHARED ?= -shared
LDLIBS ?= -ldl

CROSS_COMPILE ?=
ifneq ($(CROSS_COMPILE),)
    CC := $(CROSS_COMPILE)gcc
    AS := $(CROSS_COMPILE)as
else
    AS := as
endif

SRC_DIR := src
INC_DIR := inc
BUILD_DIR := bin
INCLUDES := -I$(INC_DIR)

HOST_ARCH := $(shell uname -m)
TARGET_ARCH := $(shell $(CC) -dumpmachine | cut -d- -f1)
CROSS := $(if $(filter $(HOST_ARCH),$(TARGET_ARCH)),0,1)

QEMU ?= qemu-aarch64
QEMU_LD ?= /usr/aarch64-linux-gnu

ifeq ($(CROSS),1)
RUNNER = $(QEMU) -L $(QEMU_LD) -E
else
RUNNER =
endif

PASSIVE_SO = $(BUILD_DIR)/libshim.so
ACTIVE_SO = $(BUILD_DIR)/libgotpatch.so
TESTER = $(BUILD_DIR)/tester
BYPASS = $(BUILD_DIR)/bypass_syscall
START_OBJ = $(BUILD_DIR)/start.o

.PHONY: all clean run run-passive run-active run-bypass

all: $(PASSIVE_SO) $(ACTIVE_SO) $(TESTER) $(BYPASS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(PASSIVE_SO): $(SRC_DIR)/binding_hook.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) $(LDSHARED) -o $@ $< $(LDLIBS)

$(ACTIVE_SO): $(SRC_DIR)/active_hook.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) $(LDSHARED) -o $@ $< $(LDLIBS)

$(TESTER): $(SRC_DIR)/tester.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(START_OBJ): $(SRC_DIR)/start.S | $(BUILD_DIR)
	$(AS) $< -o $@

$(BYPASS): $(SRC_DIR)/bypass_syscall.c $(SRC_DIR)/minc.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC_DIR)/bypass_syscall.c \
	    $(SRC_DIR)/minc.c -o $@

run: run-passive

run-passive: $(PASSIVE_SO) $(TESTER)
	$(RUNNER) LD_PRELOAD=./$(PASSIVE_SO) ./$(TESTER)

run-active: $(ACTIVE_SO) $(TESTER)
	$(RUNNER) LD_PRELOAD=./$(ACTIVE_SO) ./$(TESTER)

run-bypass: $(BYPASS)
	$(RUNNER) ./$(BYPASS)

clean:
	rm -rf $(BUILD_DIR)

submission.zip:
	zip -r submission.zip src inc Makefile
