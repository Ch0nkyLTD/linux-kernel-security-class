# HW0 - C Practice

## Prerequisites

If you are unfamiliar with GNU make files, please read the documentation here: <https://www.gnu.org/software/make/manual/make.html#Introduction>

### Installation (Ubuntu/Debian Systems)

To compile and run this assignment on Ubuntu/Debian systems, you'll need to install the aarch64 cross-compiler and QEMU user emulation:

```bash
# Install required packages
sudo apt-get update
sudo apt-get install -y gcc-aarch64-linux-gnu qemu-user-static make

# Verify installation
aarch64-linux-gnu-gcc --version
qemu-aarch64-static --version
```

### Running ARM64 Binaries on x86_64

After building with the cross-compiler, you can run the ARM64 binaries using QEMU user emulation:

```bash
# Build your program
make activate

# Run using QEMU user emulation
qemu-aarch64-static -L /usr/aarch64-linux-gnu/ ./bin/activate activate

# Alternative: if you have binfmt-support configured, you can run directly
./bin/activate activate
```

The `-L /usr/aarch64-linux-gnu/` flag tells QEMU where to find the ARM64 system libraries.

## Grading Breakdown

This assignment is worth **100 points** total, distributed as follows:

| Question | Description | Points | Details |
|----------|-------------|--------|---------|
| **Q0** | Build Success | **5** | Your code must compile successfully |
| **Q1** | Activate/Deactivate | **18** | File creation (10) + File deletion (8) |
| **Q2** | Fibonacci | **18** | Basic test cases (12) + Overflow handling (6) |
| **Q3** | MyLS | **9** | Directory listing functionality |
| **Q4** | Highground | **20** | Name greeting (10) + Anakin special case (10) |
| **Q5** | Reverse | **12** | String reversal across multiple test cases |
| **Q6** | MyStrings | **18** | ASCII string extraction from binary files |

**Total: 100 points**

**Note:** All programs will be tested using aarch64 cross-compilation and QEMU emulation. Make sure your solutions work with the provided testing setup.

## C Practice

- This homework assignment is all about reviewing C.
- Many of you who have programmed with C in the past likely did so in a POSIX/Linux environment.
- Linux makes use of LibC as the runtime environment. LibC implements functions such as `printf`, `malloc`, `free`, `memcpy`...etc.

Questions 1-5 are meant to be warm-ups/review.
Question 6 is meant to be difficult but doable. It is something you might see in a coding interview style problem.

### General Directions

Please complete the coding questions as described below. You can find the starter code and Makefile in the submission directory.

For this homework, you should not have to modify the Makefile at all. The Makefile is configured to use the aarch64 cross-compiler by default.
Note that this will be autograded using QEMU emulation and aarch64-linux-gnu-gcc.
When you are satisfied with your submission, please run `make bundle` to create a bundle.zip archive.

To build a specific binary with source file ./src/foo.c, run `make foo`

### Testing Your Solutions

You can test your programs locally using the same emulation setup as the autograder:

```bash
# Build all programs
make all

# Test individual programs
qemu-aarch64-static -L /usr/aarch64-linux-gnu/ ./bin/fibonacci 10
qemu-aarch64-static -L /usr/aarch64-linux-gnu/ ./bin/reverse hello world
qemu-aarch64-static -L /usr/aarch64-linux-gnu/ ./bin/myls .
```

### Question 1

**Activation**

Malware analyzed in this class will always search for the file `/tmp/ch0nky.txt`

If the file does not exist, the malware will not run. There will be situations where it will be convenient to create or delete this file. In particular, if we want to prevent course malware from running on our device, we could simply delete the file in that directory. To enable the malware to run again, we activate our machine by creating the file.

In this problem, you will create a command line utility that takes a single argument: either `activate` or `deactivate`.

If the argument is `activate`, the program attempts to create `/tmp/ch0nky.txt` and write the following string `"This is testing for fprintf...\n"` (No quotes)
If `deactivate` is set, the program attempts to delete `/tmp/ch0nky.txt`.

For this problem, you must use the POSIX file API contained in `stdio.h`
See <https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm> for more on the API.
To open a file, use `fopen(filename, mode)` where the `mode` values are identical to those in python. For example, to create the file in python, we would run

```python
with open("/tmp/ch0nky.txt", "w+") as f:
    f.write("This is testing for fprintf...\n")
print("Wrote file!")
```

To write data to a file, use `fprintf` defined in `stdio.h`

### Question 2

**Fibonacci**
Complete the following code to compute and print the nth Fibonacci number.
See [https://en.wikipedia.org/wiki/Fibonacci_number](https://en.wikipedia.org/wiki/Fibonacci_number) for the definition of what the Fibonacci numbers are.

- use `atoi` to convert a command line argument (`char*` into an `int`)
- when printing the value, use `printf("%d\n", nthFib);`
- Explain in the comments why running the binary with argument 47 outputs a negative number. For example
- `// MY ANSWER HERE: it outputs a negative number because of foo bar`

### Question 3

**MyLS**

Complete the program below to implement a simple version of the `ls` command utility (list files).
The program should take 1 (optional argument): a path.
The program then lists all files and directories in the specified path. If none is provided, the default path should be `.`: the current working directory.
For example

```bash
myls  /
.
..
ch0nky.txt
...
```

### Question 4

**Greeter**
Create a program that greets every name passed in the command line, unless their name is "Anakin", in which case, it prints `"ITS OVER ANAKIN! I HAVE THE HIGH GROUND\n"`
Greeting is of the form `Hello there {name}!\n`

Example:

```bash
highground  Greg Samantha
Hello there Greg!
Hello there Samantha!
highground  Robert Anakin Louis
Hello there Robert!
ITS OVER ANAKIN! I HAVE THE HIGH GROUND
Hello there Louis!
```

### Question 5

**Reverse**
Implement a program that reads multiple strings as command line arguments. For each argument, print the reversed value.
Below is a valid implementation in python.

```python
import sys

for arg in sys.argv[1:]:
    print(arg[::-1])
```

### Question 6 (HARD)

In this problem, you are going to implement the `strings` utility to parse ASCII strings from arbitrary files.
Here, an ASCII string is any contiguous array of bytes with minimum byte value 32 and maximum byte value 126 that is *null terminated*. A null terminator is hex value `0x00`, decimal value `0`, and written as a character literal `\0`.

For example, the C string "hello!" is actually an array of chars `char hello[] = {104, 101, 108, 108, 111, 33, 0};` with hex values `0x68 0x65 0x6c 0x6c 0x6f 0x21 0x00`

Your job is to load the entire file into memory, and scan it for ASCII strings.
You may assume the maximum file size is 1GB

- Step 1) Calculate the size of the file. You may use the POSIX C API to seek to the end of the file, to calculate the file size.
- Step 2) Read the entire file into a buffer on the Heap. Use `char* buffer = (char*) malloc(file_size)` to allocate enough space for the full file and read the bytes into the buffer.
- Step 3) Search the buffer for all ASCII strings that are at least 4 characters long! Here, a character is any value in the ASCII range. For a refresher on ASCII, see <https://www.w3schools.com/charsets/ref_html_ascii.asp>
  - Recall you can use python to check encoding of specific characters:

```python
In [2]: chr(100)
Out[2]: 'd'
In [3]: ord('d')
Out[3]: 100
```

Each time a string is found, print the string as follows `printf("<string>%s</string>\n", str);`

- Note I will be auto-grading this. It is important to ensure that your output is the same format!

Hints:

- You should leverage pointer arithmetic here.
- When calling `printf("%s", str)` recall that `str` is just a char pointer-- in other words, it is a memory address! It will continue writing characters to stdout until it reaches a null terminator `\0`.
- Try implementing this in python first if you are having trouble! see (optional) python starter code
- Windows Encodes text files as unicode. To test this, feel free to use the code below to generate test cases.
- Again, the python implementation is totally optional, but also good practice :-)

```python
import os

# make a test file
def make_file(file_name):
    with open(file_name, "wb+") as f:
        data = b"Hello\0I am a c string!\0I am another c string!\0" + os.urandom(500) + b"I am another c string!\0"
        f.write(data)

# steps 1 and 2
def load_file(file_name):
    with open(file_name, "rb") as f:
        data = f.read()
    return data

# min/max values
ascii_min = 32
ascii_max = 126
null = 0

# step 3:
# this is a coding interview style question :)
```

