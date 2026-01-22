# AArch64 Syscall Programming Assignment

## Assignment Overview

In this assignment, you'll implement multiple programs that communicate directly with the Linux kernel using raw system calls on the AArch64 (ARM64) architecture. This is low-level systems programming without any library helpers.

 You'll learn:

- How programs communicate with the operating system at the lowest level
- AArch64 assembly language and calling conventions
- File I/O operations using only kernel interfaces
- Memory management through direct syscalls
- Binary data processing and pattern matching
- System process memory layout analysis
- Buffered file parsing with edge case handling

Constraints:

- No `printf`, `malloc`, `fopen`, or any libc functions. Your binary must be static (this is handled in the Makefile)
- No standard library helpers (-nostdlib compilation)
- No high-level language features
- Must use custom bootstrap code (_start entry point)

You must use:

- Direct syscalls via inline assembly (`svc #0`)
- Manual memory management with `mmap`
- Raw file I/O with `openat`, `read`, `write`, `pread64`...etc
- Your own string/number parsing functions
- Custom minimal C library implementation

## Project Structure

```
.
├── README.md              # This file
├── Makefile              # Build configuration
├── src/
│   ├── start.S           # Bootstrap assembly (_start entry point)
│   ├── bgrep.c           # Binary grep implementation
│   ├── proc-maps.c       # Process memory map parser
│   ├── minc.c            # Minimal C library implementation
│   └── [other programs] # Additional syscall programs
├── inc/
│   ├── syscall_utils.h   # Core syscall framework
│   └── minc.h            # Minimal C library header
```

## Prerequisites

Before you can implement any programs, you must understand, and  write the some helper code. Every program you compile will link with `start.o`, use your syscall framework, and depend on your minimal C library.

## Implementation Guide

**Prerequisites**: Before attempting the programming assignments, you must complete the foundational infrastructure in this exact order. These components are dependencies for all subsequent work.

### Step 1: `argc,argv` Bootstrap Process (`start.S`)

The `start.S` file provides the `_start` entry point that replaces the standard C runtime initialization. Understanding this is **essential** - you will be quizzed on this material.

#### How Bootstrap Works

1. **Kernel Setup**: When Linux executes your program, it places `argc`, `argv`, and environment variables on the stack
2. **Stack Layout**: The kernel follows a specific convention documented in the Linux kernel source
3. **Entry Point**: Your `_start` function receives control with this stack layout
4. **Transition to C**: `start.S` extracts `argc` and `argv` from the stack and calls  `main(argc, argv,..)`

#### Reference Material - STUDY THIS

Study how the Linux kernel prepares the initial stack layout:

- [Linux Kernel `create_elf_tables()` function](https://github.com/torvalds/linux/blob/master/fs/binfmt_elf.c#L310)
- Look for the stack layout comments in `binfmt_elf.c`
- Understand the ELF auxiliary vector and how `argc`/`argv` are positioned

**Quiz Topics**: You will be tested on your understanding of:

- Stack layout at program startup
- How `argc` and `argv` are extracted from the stack
- The role of the ELF auxiliary vector
- AArch64 calling conventions and register usage

### Step 2: Complete the Syscall Framework (`syscall_utils.h`) - REQUIRED

This is the **core requirement** of the assignment. You must implement the missing syscall infrastructure before proceeding to any programming problems.

#### What's Provided

You are given these starter implementations:

- `syscall3()` - Three-argument syscall wrapper
- `sys_write()` - Write system call wrapper
- All syscall numbers (`SYS_openat`, `SYS_close`, etc.)
- Constants and type definitions (`AT_FDCWD`, `O_RDONLY`, etc.)

#### What You Must Implement

You must implement these missing syscall wrappers using AArch64 inline assembly:

```c
// Implement these functions in syscall_utils.h:
static inline long syscall0(long num);
static inline long syscall1(long num, long arg1);
static inline long syscall2(long num, long arg1, long arg2);
static inline long syscall4(long num, long arg1, long arg2, long arg3, long arg4);
static inline long syscall5(long num, long arg1, long arg2, long arg3, long arg4, long arg5);
static inline long syscall6(long num, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6);
```

**Reference the provided `syscall3()` implementation** to understand the pattern:

- Syscall number goes in register `x8`
- Arguments go in registers `x0`, `x1`, `x2`, `x3`, `x4`, `x5`
- Return value comes back in `x0`
- Use `svc #0` instruction to invoke the syscall

#### Required Syscall Wrapper Functions

After implementing the basic syscall wrappers, implement these higher-level syscall functions:

```c
// File operations
static inline int sys_openat(int dirfd, const char *pathname, int flags, mode_t mode);
static inline int sys_close(int fd);
static inline ssize_t sys_read(int fd, void *buf, size_t count);
static inline ssize_t sys_pread64(int fd, void *buf, size_t count, off_t offset);
static inline int sys_fstat(int fd, struct stat *statbuf);

// Memory management
static inline void *sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
static inline int sys_munmap(void *addr, size_t length);
```

### Step 3: Implement Minimal C Library (`minc.h`/`minc.c`) - REQUIRED

Since you cannot use standard library functions, you must implement basic C library functionality before writing any application code.

#### Implementation Approach

You have **permission to research and adapt existing implementations** from online sources. However:

- **Mandatory Practice**: You must implement `memcpy()` and `strlen()` yourself as practice. I will quiz you on this.
- **Other Functions**: For remaining functions, you may adapt implementations from reliable sources (read as you can copy and paste :-) )
- **Citation**: Comment your code with the source if you adapt existing implementations
- **No submitting code you don't understand**: Ensure you understand any code you adapt/paste. I reserve the right to quiz you on any code you submit.

#### Required Functions

Implement these functions in `src/minc.c`:

```c
// String functions (you may adapt from online sources except strlen)
size_t strlen(const char *s);                    // MUST implement yourself
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
char *strstr(const char *haystack, const char *needle);
char *strchr(const char *s, int c);

// Memory functions (you may adapt from online sources except memcpy)
void *memcpy(void *dest, const void *src, size_t n);  // MUST implement yourself
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

// Conversion and utility functions (you may adapt from online sources)
int atoi(const char *str);
long atol(const char *str);

// these are helpful for debugging: parse an uppercase/lowercase hex string  and return its int representation
long parse_hex(const char *hex_str);
// debug: write the str representation of an integer 
void write_num(int fd, long num);
// debug write the hex value of an integer 
void write_hex(int fd, unsigned long num);
```

#### Order of operations

1. **Start with mandatory functions**: Implement `strlen()` and `memcpy()` from scratch
2. **Research implementations**: Find reference implementations for other functions
3. **Adapt and test**: Modify implementations to fit your needs and test thoroughly
4. **Document sources**: Add comments citing where you found reference implementations

- i.e. `// stolen from https://foobar`

### Validation and Testing

Before proceeding, verify the boiler plate works:
For example:

```c
// Test program to verify your implementations
int main() {
    char test[] = "Hello";
    sys_write(1, test, strlen(test));  // Should work if both are implemented
    return 0;
}
```

#### Test Your Syscall Header

Create simple test programs to verify each syscall works: For example,

```c
// Test sys_write
int main() {
    char msg[] = "Hello from syscalls!\n";
    sys_write(1, msg, 21);
    return 0;
}

// Test sys_openat and sys_read
int main() {
    int fd = sys_openat(AT_FDCWD, "/etc/hostname", O_RDONLY, 0);
    if (fd >= 0) {
        char buf[64];
        ssize_t n = sys_read(fd, buf, sizeof(buf));
        sys_write(1, buf, n);
        sys_close(fd);
    }
    return 0;
}
```

#### Test Your Minimal C Library

Verify each function works correctly: For example,

```c
int main() {
    char str1[] = "Hello";
    char str2[] = " World";
    char result[20];

    // Test string functions
    strcpy(result, str1);
    strcat(result, str2);
    sys_write(1, result, strlen(result));

    return 0;
}
```

## Main Assignments

**Prerequisites**: You must complete ALL pre-reqs (Steps  above) before continuing  

### Assignment 1: Binary Grep (`bgrep`)

Implement a binary pattern search tool that:

1. Opens a file using your syscalls
2. Searches for hex patterns (e.g., `DEADBEEF`) in the file
3. Reports all byte offsets where the pattern is found
4. Uses chunked reading with `pread64` and a buffer that is 4096 bytes (stack allocated)
5. Be careful with patterns that exist at boundaries. I.e. if the end of the buffer is `DEA` and the start of the next chunk is `DBEEF` you need to handle this.

#### Usage

```bash
./bin/bgrep <file> <hexpattern>
```

#### Examples

```bash
# Search for the hex bytes DE AD BE EF in a file
./bin/bgrep myfile.bin DEADBEEF

# Expected output (if pattern found at offset 100):
Found at offset: 100 (0x64)

# Multiple matches show all occurrences:
Found at offset: 6 (0x6)
Found at offset: 42 (0x2A)
```

#### Requirements

- Handle binary files (not just text)
- Work on files larger than available RAM
- Find all occurrences of the pattern
- Use chunked reading with proper overlap handling
- Parse hex strings manually using your `parse_hex()` function
- Output both decimal and hex offsets using your `write_num()` and `write_hex()` functions
- Return exit code 1 if no matches found
- Validate hex patterns and show appropriate error messages

### Assignment 2: Process Memory Map Parser (`proc-maps`)

Implement a `/proc/self/maps` parser that:

1. Opens `/proc/self/maps` using your syscalls
2. Parses each line to extract memory region information
3. Uses buffered reading with a fixed 4096-byte buffer
4. Handles lines that span buffer boundaries correctly

#### Usage

```bash
./bin/proc-maps
```

#### Expected Output

```
=== Process Memory Map ===
Start: 0x400000
End:   0x402000
Perms: r-xp
Name:  /bin/proc-maps
---
Start: 0x5500000000
End:   0x5500001000
Perms: ---p
---
```

#### Requirements

- Use exactly 4096-byte buffer for reading
- Handle partial lines at buffer boundaries
- Parse address ranges, permissions, and pathnames using your string functions
- Implement line-by-line processing without losing data
- Extract fields from `/proc/self/maps` format: `start-end perms offset dev inode pathname`

### Assignment 3: File Timestamp Modification (`timestomp-lite`)

Change a file's **atime** and **mtime** timestamps using direct syscalls.

#### Usage

```bash
./bin/timestomp-lite <file> --at <epoch> --mt <epoch>
./bin/timestomp-lite <file> --copy-from <otherfile>
```

#### Requirements

- Use `utimensat` syscall to modify timestamps
- Support Unix epoch format for timestamp values
- Implement copy-from mode to copy timestamps from another file
- Handle errors gracefully with appropriate error messages
- Use only direct syscalls (no libc time functions)

### Assignment 4: Word Count via Memory Mapping (`wc-mmap`)

Count bytes, words, and lines in a file using memory mapping with optional flags.

#### Usage

```bash
./bin/wc-mmap [-l|-c] <file>
```

#### Options

- `-l` : Count lines only
- `-c` : Count bytes only
- No flag : Count lines, words, and bytes (default)
  - What is a word? `man wc`

#### Expected Output

```bash
# Default (no flags)
./bin/wc-mmap file.txt
2 4 21 file.txt

# Lines only
./bin/wc-mmap -l file.txt
2 file.txt

# Bytes only
./bin/wc-mmap -c file.txt
21 file.txt
```

#### Requirements

- Must use `mmap` for file access
- Support `-l` and `-c` command-line flags
- Count lines, words, and bytes accurately
- Handle large files efficiently
- Match standard `wc` output format for respective flags
- Use direct syscalls for all operations
- Implement proper argument parsing without getopt

### Assignment 5: Hash Computation (`djb2sum`)

Compute a DJB2 hash of a file's contents.

#### Usage

```bash
./bin/djb2sum <file>
```

#### Requirements

- Implement DJB2 hash algorithm
- Work with both small and large files
- Output hash value in hexadecimal format
- Use either `read` or `mmap` for file access
- Handle binary files correctly

### Assignment 6: Structured Logging System (`alogappend`, `aloglist`)

Create a structured log format with fixed-size entries.

#### Usage

```bash
# Append entries
./bin/alogappend <logfile> <id> <message>

# List entries
./bin/aloglist <logfile>
```

#### Entry Format

Each log entry is exactly 256 bytes:

- `u64 id` (8 bytes)
- `u64 timestamp` (8 bytes)
- `u16 message_length` + padding (8 bytes)
- `char message[240]` (240 bytes)

#### Requirements

- Use `O_APPEND` for atomic log writes
- Parse existing log files correctly
- Timestamp is simulated in the starter code
- Validate entry format consistency

### Assignment 7: Minimal Printf (`log_s`)

Implement a printf-like function that only supports `%s` format specifier.

#### Usage

```bash
./bin/log_s <fd> "<format_string>" "<argument>"
```

#### Requirements

- Replace all `%s` in format string with argument
- Support multiple `%s` occurrences
- Use `mmap` for buffer allocation (no malloc)
- Write output using `write` syscall only
- Handle buffer size limits appropriately
- Example fd path: `/proc/self/1`

### Assignment 8: Process Memory Map Display (`pmaps`)

Print the contents of `/proc/self/maps` to stdout.

#### Usage

```bash
./bin/pmaps
```

#### Requirements

- Open and read `/proc/self/maps`
- Output raw contents to stdout
- Use either `read` or `mmap` for file access
- Handle `/proc` filesystem quirks
- Implement proper error handling

### Assignment 9: Execute Raw Machine Code (`runrwx`)

Load raw machine code from a file and execute it.

#### Usage

```bash
./bin/runrwx <codefile> [args...]
```

#### Test Program

A test program `testsc.bin` is provided that demonstrates the functionality:

```bash
# Build the test binary
make testsc.bin

# Run the test
./bin/runrwx bin/testsc.bin
```

The test program (`testsc.S`) uses stack strings to build "hello world!\n" and writes it to stdout, then returns 42.

#### Requirements

- Map file with `PROT_READ|PROT_WRITE|PROT_EXEC` permissions
- Call loaded code as function with signature `int(int argc, char **argv)`
- Exit with the return value from executed code
- Implement proper cleanup with `munmap`
- Display status messages before and after execution

### Assignment 10: Text Search and Replace (`sed-lite`)

Simplified `sed` implementation for find/replace operations.

#### Usage

```bash
./bin/sed-lite <file> <search> <replace>
```

#### Requirements

- Map file into memory using `mmap`
- Replace all occurrences of search string with replacement
- Handle different length search/replace strings
- Write modified content to stdout
- Empty search string should be treated as an error

## Building and Testing

### Build Commands

```bash
# Build all programs
make all

# Programs will be created in the bin/ directory:
# - bin/bgrep
# - bin/proc-maps
# - [other programs]
```

### Testing Your Implementation

#### Test bgrep

```bash
# Create a test file with a known pattern
echo -e "Hello \xDE\xAD\xBE\xEF World" > test.bin

# Test your implementation
./bin/bgrep test.bin DEADBEEF
# Should output: Found at offset: 6 (0x6)

# Test multiple matches
echo -e "\xFFHello\xFFWorld\xFF" > multi.bin
./bin/bgrep multi.bin FF
# Should output:
# Found at offset: 0 (0x0)
# Found at offset: 6 (0x6)
# Found at offset: 12 (0xC)
```

#### Test proc-maps

```bash
# Test the memory map parser
./bin/proc-maps
# Should output formatted memory regions of the process
```

#### Test error cases

```bash
# Test invalid hex
./bin/bgrep test.bin INVALID
# Should output: error: invalid hex pattern

# Test empty pattern
./bin/bgrep test.bin ""
# Should output: error: empty hex pattern

# Test missing arguments
./bin/bgrep
# Should output: Usage: bgrep <file> <hexpattern>
```

#### Test large files

```bash
# Create a 10MB file with pattern at known location
dd if=/dev/zero of=largefile.bin bs=1M count=10
printf '\xDE\xAD\xBE\xEF' | dd of=largefile.bin bs=1 seek=5000000 conv=notrunc

./bin/bgrep largefile.bin DEADBEEF
# Should find pattern at offset 5000000
```

### Common Build Issues

- **"SYS_* undeclared"** - Define the syscall numbers in `syscall_utils.h`
- **"undefined reference to _start"** - Make sure `src/start.S` is being assembled and linked
- **"undefined reference to strlen/memcpy/etc"** - Implement these functions in `src/minc.c`

## Submission

### What to Submit

Submit your complete project directory containing:

- `src/` - All your source files (bgrep.c, proc-maps.c, minc.c, start.S)
- `inc/` - All your header files (syscall_utils.h, minc.h)
- `Makefile` - Build configuration (if modified)

### Autograder Details

The autograder will:

1. Cross-compile your code with `aarch64-linux-gnu-gcc`
2. Run tests under QEMU emulation with `qemu-aarch64-static`
3. Verify your binaries are truly syscall-only (no libc dependencies)
4. Test both programs with various inputs and edge cases
5. Check exit codes and error message formats

Note: The autograder will likely not work properly until after the assignment is due.

## Debugging Tips

### Use GDB for Assembly Debugging

```bash
# Debug with GDB (if available)
gdb ./bin/bgrep
(gdb) break main
(gdb) run testfile.bin DEADBEEF
(gdb) stepi  # Step through assembly
```

### Check Syscall Numbers

```bash
# Or check the kernel headers
grep -r "define.*SYS_openat" /usr/include/
```

### Debug Syscall Issues

```bash
# Use strace to see what syscalls your program makes
strace -e trace=openat,read,write,pread64 
```

### Test Individual Components

- Test your syscall wrappers with simple programs first
- Verify your string functions work before using them in the main programs
- Test error cases thoroughly (file not found, invalid input, etc.)

## Resources

### AArch64 Assembly References

- [ARM Developer Documentation](https://developer.arm.com/documentation/)
- [AArch64 Instruction Set Reference](https://developer.arm.com/architectures/instruction-sets/base-isas/aarch64)
- [Linux AArch64 ABI](https://github.com/ARM-software/abi-aa)

### Linux Syscall Information

- [Linux Syscall Reference](https://syscalls.kernelgrok.com/)
- [man 2 openat](https://man7.org/linux/man-pages/man2/openat.2.html)
- [man 2 pread](https://man7.org/linux/man-pages/man2/pread.2.html)
- [AArch64 syscall table](https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/unistd.h)

### Autograder  Environment

- Cross-compilation with `aarch64-linux-gnu-gcc`
- Emulation with `qemu-aarch64-static`
- Testing under `/opt/aarch64-rootfs` environment
