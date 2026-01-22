# Makefile for C/C++ projects with bin/src/inc structure
CC ?= gcc
CXX ?= g++
CFLAGS ?= -O2 -Wall -Wextra -std=c99
CXXFLAGS ?= -O2 -Wall -Wextra -std=c++17
LDFLAGS ?= -static

# Cross-compilation support
CROSS_COMPILE ?= aarch64-linux-gnu-
ifneq ($(CROSS_COMPILE),)
    CC := $(CROSS_COMPILE)gcc
    CXX := $(CROSS_COMPILE)g++
endif

# Directories
SRCDIR := src
INCDIR := inc
BINDIR := bin
OBJDIR := obj

# Program targets
PROGRAMS := bgrep timestomp-lite wc-mmap djb2sum alogappend aloglist log_s pmaps runrwx sed-lite proc-maps hello
PROGRAM_TARGETS := $(addprefix $(BINDIR)/, $(PROGRAMS))

# Default target (for backwards compatibility)
TARGET ?= $(BINDIR)/bgrep
OUT ?= $(TARGET)

# Source files
CSRCS := $(wildcard $(SRCDIR)/*.c)
CXXSRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.cc $(SRCDIR)/*.cxx)
COBJS := $(CSRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
CXXOBJS := $(CXXSRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) \
           $(CXXSRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o) \
           $(CXXSRCS:$(SRCDIR)/%.cxx=$(OBJDIR)/%.o)
OBJS := $(COBJS) $(CXXOBJS)

# Include directories
INCLUDES := -I$(INCDIR)

# Default target
.PHONY: all clean $(PROGRAMS) testsc.bin

# Build all programs and test binary
all: $(PROGRAM_TARGETS) $(BINDIR)/testsc.bin

# Individual program targets
$(PROGRAMS): %: $(BINDIR)/%

# All programs use start.S bootstrap for pure syscall programming
$(BINDIR)/start.o: $(SRCDIR)/start.S | $(BINDIR)
	$(CROSS_COMPILE)as $(SRCDIR)/start.S -o $@

# Build testsc.bin for runrwx testing
$(BINDIR)/testsc.bin: $(SRCDIR)/testsc.S | $(BINDIR)
	$(CROSS_COMPILE)as $(SRCDIR)/testsc.S -o $(BINDIR)/testsc.o
	$(CROSS_COMPILE)objcopy -O binary -j .text $(BINDIR)/testsc.o $@
	rm -f $(BINDIR)/testsc.o

# Add testsc.bin to default targets
testsc.bin: $(BINDIR)/testsc.bin

# Generic rule for all programs (including hello)
$(BINDIR)/%: $(SRCDIR)/%.c $(BINDIR)/start.o $(SRCDIR)/minc.c | $(BINDIR)
	$(CC) -nostdlib -O0 -fPIE -static -e _start $(INCLUDES) $(BINDIR)/start.o $< $(SRCDIR)/minc.c -o $@

# Create directories
$(BINDIR) $(OBJDIR):
	mkdir -p $@

# Legacy support - remove the old compilation rules since we use direct compilation now

# Clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Package submission for Gradescope
submission.zip:
	zip -r submission.zip src inc Makefile
