/* log_s.c - Lazy printf supporting only %s using direct syscalls */
#include "syscall_utils.h"
#include "minc.h"

#define BUFFER_SIZE 4096

    // Your solution here!

int main(int argc, char **argv) {
    if (argc != 4) {
        const char *usage = "Usage: log_s <fd> \"<fmt>\" \"<arg>\"\n";
        sys_write(2, usage, strlen(usage));
        return 1;
    }

    const char *fd_str = argv[1];
    const char *fmt = argv[2];
    const char *arg = argv[3];

    // Your solution here!
    return 0;
}