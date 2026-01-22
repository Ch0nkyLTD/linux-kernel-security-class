/* sed-lite.c - Simple find/replace using direct syscalls */
#include "syscall_utils.h"
#include "minc.h"

#define OUTPUT_BUFFER_SIZE 8192

    // Your solution here!

int main(int argc, char **argv) {
    if (argc != 4) {
        const char *usage = "Usage: sed-lite <file> <search> <replace>\n";
        sys_write(2, usage, strlen(usage));
        return 1;
    }

    const char *filename = argv[1];
    const char *search = argv[2];
    const char *replace = argv[3];

    // Your solution here!
    return 0;
}