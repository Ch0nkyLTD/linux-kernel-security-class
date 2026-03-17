# Shellcode Loaders — In-Class Exercises Makefile
HOST_ARCH := $(shell uname -m)

ifeq ($(HOST_ARCH),x86_64)
    CC      := aarch64-linux-gnu-gcc
    AS      := aarch64-linux-gnu-as
    OBJCOPY := aarch64-linux-gnu-objcopy
    READELF := aarch64-linux-gnu-readelf
    CROSS   := 1
else
    CC      := gcc
    AS      := as
    OBJCOPY := objcopy
    READELF := readelf
    CROSS   := 0
endif

# ── QEMU runner (only when cross-compiling) ────────────────────────
ifeq ($(CROSS),1)
    RUN := qemu-aarch64 -L /usr/aarch64-linux-gnu
else
    RUN :=
endif

# ── Common flags ───────────────────────────────────────────────────
CFLAGS      := -Wall -Wextra -g
INC         := -Iinc
BINDIR      := bin
NOLIBC_FLAGS := -nostdlib -nostartfiles -fPIC -fno-stack-protector \
                -fomit-frame-pointer -fno-asynchronous-unwind-tables

# ── Phony targets ──────────────────────────────────────────────────
.PHONY: all clean payload ex1 ex2 ex3-proto ex3 ex4 \
        run-ex1 run-ex2 run-ex3-proto-libc run-ex3-proto-sym run-ex3 run-ex4

all: payload ex1 ex2 ex3-proto ex3

$(BINDIR):
	mkdir -p $(BINDIR)

# ── Payload SO → xxd header ────────────────────────────────────────
$(BINDIR)/libinitlib.so: payload/initlib.c | $(BINDIR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

inc/payload_bytes.h: $(BINDIR)/libinitlib.so
	xxd -i $< > $@.tmp
	@# Normalize the symbol names (strip path prefix)
	sed 's/bin_libinitlib_so/payload_bytes/g' $@.tmp > $@
	rm -f $@.tmp

payload: inc/payload_bytes.h

# ── Exercise 1: /tmp + dlopen ──────────────────────────────────────
$(BINDIR)/ex1: ex1_tmp_loader/main.c inc/payload_bytes.h | $(BINDIR)
	$(CC) $(CFLAGS) $(INC) -o $@ $< -ldl

ex1: $(BINDIR)/ex1

run-ex1: $(BINDIR)/ex1
	@echo "\n========================================"
	@echo " Ex1: /tmp + dlopen"
	@echo "========================================\n"
	$(RUN) ./$(BINDIR)/ex1

# ── Exercise 2: /dev/shm + dlopen ─────────────────────────────────
$(BINDIR)/ex2: ex2_shm_loader/main.c inc/payload_bytes.h | $(BINDIR)
	$(CC) $(CFLAGS) $(INC) -o $@ $< -ldl

ex2: $(BINDIR)/ex2

run-ex2: $(BINDIR)/ex2
	@echo "\n========================================"
	@echo " Ex2: /dev/shm + dlopen"
	@echo "========================================\n"
	$(RUN) ./$(BINDIR)/ex2

# ── Exercise 3: C prototypes (with libc) ──────────────────────────
$(BINDIR)/find_libc: ex3_c_prototype/find_libc.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BINDIR)/find_symbol: ex3_c_prototype/find_symbol.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $< -ldl

ex3-proto: $(BINDIR)/find_libc $(BINDIR)/find_symbol

run-ex3-proto-libc: $(BINDIR)/find_libc
	@echo "\n========================================"
	@echo " Ex3 Prototype: find libc base"
	@echo "========================================\n"
	$(RUN) ./$(BINDIR)/find_libc

run-ex3-proto-sym: $(BINDIR)/find_symbol
	@echo "\n========================================"
	@echo " Ex3 Prototype: find symbol in ELF"
	@echo "========================================\n"
	$(RUN) ./$(BINDIR)/find_symbol

# ── Exercise 3: no-libc loader (shared object + tester) ──────────
# The loader is a -nostdlib SO loaded via LD_PRELOAD into the tester
# binary, which guarantees libc is mapped in the process.

$(BINDIR)/ex3.so: ex3_nolibc_loader/loader.c \
                  ex3_nolibc_loader/procfsutil.h ex3_nolibc_loader/elfsym.h \
                  inc/payload_bytes.h | $(BINDIR)
	$(CC) $(NOLIBC_FLAGS) -shared $(INC) -Iex3_nolibc_loader \
		ex3_nolibc_loader/loader.c \
		-o $@

$(BINDIR)/ex3_tester: ex3_nolibc_loader/tester.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $<

ex3: $(BINDIR)/ex3.so $(BINDIR)/ex3_tester

run-ex3: $(BINDIR)/ex3.so $(BINDIR)/ex3_tester
	@echo "\n========================================"
	@echo " Ex3: no-libc memfd loader"
	@echo "========================================\n"
ifeq ($(CROSS),1)
	qemu-aarch64 -L /usr/aarch64-linux-gnu -E LD_PRELOAD=./$(BINDIR)/ex3.so ./$(BINDIR)/ex3_tester
else
	LD_PRELOAD=./$(BINDIR)/ex3.so ./$(BINDIR)/ex3_tester
endif

# ── Exercise 4: bootstrap shellcode ────────────────────────────────

# Build the payload SO (uses libc, has constructors)
$(BINDIR)/ex4_payload.so: ex4_bootstrap/payload.c | $(BINDIR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

# Build the reflective loader SO (no-libc, entry = load_memfd)
$(BINDIR)/rp.so: ex4_bootstrap/loader.c \
                 ex4_bootstrap/payload.c \
                 solutions/ex3_procfsutil_solution.h \
                 solutions/ex3_elfsym_solution.h | $(BINDIR)
	$(CC) -c $(NOLIBC_FLAGS) $(INC) -Isolutions \
		ex4_bootstrap/loader.c \
		-o $(BINDIR)/loader.o
	$(CC) -fPIC -shared $(INC) \
		$(BINDIR)/loader.o \
		ex4_bootstrap/payload.c \
		-Wl,-entry=load_memfd \
		-o $@

# Generate bootstrap stub, assemble, concatenate
$(BINDIR)/mempayload: $(BINDIR)/rp.so ex4_bootstrap/bootstrap.py | $(BINDIR)
	python3 ex4_bootstrap/bootstrap.py $(BINDIR)/rp.so > $(BINDIR)/bootstrap.S
	$(AS) $(BINDIR)/bootstrap.S -o $(BINDIR)/bootstrap.o
	$(OBJCOPY) -O binary -j .text $(BINDIR)/bootstrap.o $(BINDIR)/bootstrap.bin
	cat $(BINDIR)/bootstrap.bin $(BINDIR)/rp.so > $@

# Build test harness
$(BINDIR)/test_harness: ex4_bootstrap/test_harness.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $<

ex4: $(BINDIR)/mempayload $(BINDIR)/test_harness

run-ex4: $(BINDIR)/mempayload $(BINDIR)/test_harness
	@echo "\n========================================"
	@echo " Ex4: bootstrap shellcode"
	@echo "========================================\n"
	$(RUN) ./$(BINDIR)/test_harness ./$(BINDIR)/mempayload

# ── Clean ──────────────────────────────────────────────────────────
clean:
	rm -rf $(BINDIR) inc/payload_bytes.h
