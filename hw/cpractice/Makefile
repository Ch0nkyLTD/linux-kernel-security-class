# Compiler and Tools
ARCH := aarch64-linux-gnu
CC := $(ARCH)-gcc
LD := $(ARCH)-ld

# Directories
SRC_DIR := ./src
INC_DIR := ./inc
BUILD_DIR := ./build
BIN_DIR := ./bin

# Source files and corresponding object and binary files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
BINS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRCS))

# Compiler and linker flags
CFLAGS := -Wall -O2 -I$(INC_DIR)
LDFLAGS :=

# Default target: build all binaries
.PHONY: all clean bundle

all: $(BINS)

# Individual binary targets
$(BIN_DIR)/%: $(BUILD_DIR)/%.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $<

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# make bundle submission
bundle:
	zip -r bundle.zip Makefile inc src

# Clean build artifacts
clean:
	-rm -rf $(BUILD_DIR) $(BIN_DIR)
	-rm bundle.zip

# Add shortcuts for each binary
$(notdir $(BINS)): %: $(BIN_DIR)/%
	@echo "Built $@ successfully."