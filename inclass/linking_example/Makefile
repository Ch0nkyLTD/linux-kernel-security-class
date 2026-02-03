# Linking Examples Makefile -- aarch64 (native or cross-compiled)

# ── Architecture detection ─────────────────────────────────────────
HOST_ARCH := $(shell uname -m)
TARGET_ARCH := aarch64

ifeq ($(HOST_ARCH),x86_64)
    CC    := aarch64-linux-gnu-gcc
    CROSS := 1
else
    CC    := gcc
    CROSS := 0
endif

# ── QEMU runner (only when cross-compiling) ────────────────────────
ifeq ($(CROSS),1)
    RUN     = qemu-aarch64 -L /usr/aarch64-linux-gnu
    RUN_ENV = qemu-aarch64 -L /usr/aarch64-linux-gnu -E
else
    RUN     =
    RUN_ENV =
endif

# ── Common flags ───────────────────────────────────────────────────
CFLAGS  := -Wall -Wextra -g
PIC     := -fPIC
BINDIR  := bin

# ── Phony targets ──────────────────────────────────────────────────
.PHONY: all clean run run-ex1 run-ex2 run-ex3 run-ex4 run-ex5 bundle

# ── Default: build everything ──────────────────────────────────────
all: $(BINDIR)/libmylib.so    $(BINDIR)/ex1_main \
     $(BINDIR)/libinitlib.so  $(BINDIR)/ex2_main \
     $(BINDIR)/libmathlib.so  $(BINDIR)/ex3_main \
     $(BINDIR)/libwraplib.so  $(BINDIR)/ex4_main \
     $(BINDIR)/libpreload.so  $(BINDIR)/ex5_main

$(BINDIR):
	mkdir -p $(BINDIR)

# ── Ex1: Basic Shared Library ─────────────────────────────────────
$(BINDIR)/libmylib.so: ex1_basic_so/mylib.c ex1_basic_so/mylib.h | $(BINDIR)
	$(CC) $(CFLAGS) $(PIC) -shared -o $@ ex1_basic_so/mylib.c

$(BINDIR)/ex1_main: ex1_basic_so/main.c $(BINDIR)/libmylib.so | $(BINDIR)
	$(CC) $(CFLAGS) -Iex1_basic_so -o $@ ex1_basic_so/main.c -L$(BINDIR) -lmylib

# ── Ex2: Constructor / Destructor ──────────────────────────────────
$(BINDIR)/libinitlib.so: ex2_constructor/initlib.c | $(BINDIR)
	$(CC) $(CFLAGS) $(PIC) -shared -o $@ ex2_constructor/initlib.c

$(BINDIR)/ex2_main: ex2_constructor/main.c $(BINDIR)/libinitlib.so | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ ex2_constructor/main.c -L$(BINDIR) -linitlib

# ── Ex3: dlopen / dlsym ───────────────────────────────────────────
$(BINDIR)/libmathlib.so: ex3_dlopen/mathlib.c | $(BINDIR)
	$(CC) $(CFLAGS) $(PIC) -shared -o $@ ex3_dlopen/mathlib.c

$(BINDIR)/ex3_main: ex3_dlopen/main.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ ex3_dlopen/main.c -ldl

# ── Ex4: RTLD_DEFAULT / RTLD_NEXT ─────────────────────────────────
$(BINDIR)/libwraplib.so: ex4_symbol_walk/wraplib.c | $(BINDIR)
	$(CC) $(CFLAGS) $(PIC) -shared -o $@ ex4_symbol_walk/wraplib.c -ldl

$(BINDIR)/ex4_main: ex4_symbol_walk/main.c $(BINDIR)/libwraplib.so | $(BINDIR)
	$(CC) $(CFLAGS) -fno-builtin-printf -o $@ ex4_symbol_walk/main.c -L$(BINDIR) -lwraplib -ldl

# ── Ex5: LD_PRELOAD interposition ─────────────────────────────────
$(BINDIR)/libpreload.so: ex5_preload/preload.c | $(BINDIR)
	$(CC) $(CFLAGS) $(PIC) -shared -o $@ ex5_preload/preload.c -ldl

$(BINDIR)/ex5_main: ex5_preload/main.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ ex5_preload/main.c

# ── Run targets ────────────────────────────────────────────────────
BANNER = @echo "\n========================================" && echo " $(1)" && echo "========================================\n"

run: run-ex1 run-ex2 run-ex3 run-ex4 run-ex5

run-ex1: $(BINDIR)/ex1_main
	$(call BANNER,Ex1: Basic Shared Library)
ifeq ($(CROSS),1)
	$(RUN_ENV) LD_LIBRARY_PATH=./bin ./bin/ex1_main
else
	LD_LIBRARY_PATH=./bin ./bin/ex1_main
endif

run-ex2: $(BINDIR)/ex2_main
	$(call BANNER,Ex2: Constructor / Destructor)
ifeq ($(CROSS),1)
	$(RUN_ENV) LD_LIBRARY_PATH=./bin ./bin/ex2_main
else
	LD_LIBRARY_PATH=./bin ./bin/ex2_main
endif

run-ex3: $(BINDIR)/ex3_main
	$(call BANNER,Ex3: dlopen / dlsym)
ifeq ($(CROSS),1)
	$(RUN) ./bin/ex3_main
else
	./bin/ex3_main
endif

run-ex4: $(BINDIR)/ex4_main
	$(call BANNER,Ex4: RTLD_DEFAULT / RTLD_NEXT)
ifeq ($(CROSS),1)
	$(RUN_ENV) LD_LIBRARY_PATH=./bin ./bin/ex4_main
else
	LD_LIBRARY_PATH=./bin ./bin/ex4_main
endif

run-ex5: $(BINDIR)/ex5_main $(BINDIR)/libpreload.so
	$(call BANNER,Ex5: LD_PRELOAD Interposition)
	@echo "--- Without LD_PRELOAD ---"
ifeq ($(CROSS),1)
	$(RUN) ./bin/ex5_main
	@echo ""
	@echo "--- With LD_PRELOAD ---"
	$(RUN_ENV) LD_PRELOAD=./bin/libpreload.so ./bin/ex5_main
else
	./bin/ex5_main
	@echo ""
	@echo "--- With LD_PRELOAD ---"
	LD_PRELOAD=./bin/libpreload.so ./bin/ex5_main
endif

# ── Bundle ─────────────────────────────────────────────────────────
SOURCES := Makefile \
           ex1_basic_so/mylib.h ex1_basic_so/mylib.c ex1_basic_so/main.c \
           ex2_constructor/initlib.c ex2_constructor/main.c \
           ex3_dlopen/mathlib.c ex3_dlopen/main.c \
           ex4_symbol_walk/wraplib.c ex4_symbol_walk/main.c \
           ex5_preload/preload.c ex5_preload/main.c

bundle: clean
	tar czf bundle.tar.gz $(SOURCES)

# ── Clean ──────────────────────────────────────────────────────────
clean:
	rm -rf $(BINDIR) bundle.tar.gz
