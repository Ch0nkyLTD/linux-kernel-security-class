/* djb2sum.c - DJB2 hash computation using direct syscalls */
#include "syscall_utils.h"
#include "minc.h"

    // Your solution here!

int main(int argc, char **argv) {
    if (argc != 2) {
        const char *usage = "Usage: djb2sum <file>\n";
        sys_write(2, usage, strlen(usage));
        return 1;
    }

    const char *filename = argv[1];

    // Your solution here!
    return 0;
}