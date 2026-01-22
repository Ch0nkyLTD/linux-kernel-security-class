/* runrwx.c - Execute raw machine code using direct syscalls */
#include "syscall_utils.h"
#include "minc.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        const char *usage = "Usage: runrwx <codefile> [args...]\n";
        sys_write(2, usage, strlen(usage));
        return 1;
    }

    const char *codefile = argv[1];

    // Your solution here!
    return result;
}