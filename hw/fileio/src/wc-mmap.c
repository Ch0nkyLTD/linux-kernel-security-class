/* wc-mmap.c - Word count using memory mapping and direct syscalls */
#include "syscall_utils.h"
#include "minc.h"

    // Your solution here!

int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        const char *usage = "Usage: wc-mmap [-l|-c] <file>\n"
                           "  -l  Count lines only\n"
                           "  -c  Count bytes only\n"
                           "  (no flag = count lines, words, and bytes)\n";
        sys_write(2, usage, strlen(usage));
        return 1;
    }

    // Parse arguments
    int lines_only = 0;
    int bytes_only = 0;
    const char *filename;

    if (argc == 3) {
        // Flag provided
        if (strcmp(argv[1], "-l") == 0) {
            lines_only = 1;
        } else if (strcmp(argv[1], "-c") == 0) {
            bytes_only = 1;
        } else {
            const char *err = "Error: Invalid flag. Use -l or -c\n";
            sys_write(2, err, strlen(err));
            return 1;
        }
        filename = argv[2];
    } else {
        // No flag, default behavior
        filename = argv[1];
    }

    // Your solution here!
    return 0;
}