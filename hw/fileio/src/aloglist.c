/* aloglist.c - List entries from structured log using direct syscalls */
#include "syscall_utils.h"
#include "minc.h"

#pragma pack(1)
struct log_entry {
    uint64_t id;
    uint64_t timestamp;
    uint16_t msglen;
    uint8_t padding[6];
    char message[240];
};

int main(int argc, char **argv) {
    if (argc != 2) {
        const char *usage = "Usage: aloglist <logfile>\n";
        sys_write(2, usage, strlen(usage));
        return 1;
    }

    const char *logfile = argv[1];

    // Your solution here!
    return 0;
}