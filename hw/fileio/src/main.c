/* bgrep.c
 * Binary grep - search for hex patterns in files using pread64
 *
 * Usage: bgrep <file> <hexpattern>
 */

#include "syscall_utils.h"
#include "minc.h"
#include <stdint.h>
#include <sys/stat.h>

#define CHUNK_SIZE 4096

int main(int argc, char **argv) {
    if (argc != 3) {
        const char *u = "Usage: bgrep <file> <hexpattern>\n";
        sys_write(2, u, strlen(u));
        return 1;
    }

    const char *path = argv[1];
    const char *hex_str = argv[2];

    // Parse hex pattern
    uint8_t pattern[256];
    int pattern_len = parse_hex(hex_str, pattern, sizeof(pattern));
    if (pattern_len < 0) {
        sys_write(2, "Error: invalid hex pattern\n", 27);
        return 1;
    }
    if (pattern_len == 0) {
        sys_write(2, "Error: empty pattern\n", 21);
        return 1;
    }

    // Open file
    int fd = sys_openat(AT_FDCWD, path, O_RDONLY, 0);
    if (fd < 0) {
        sys_write(2, "Error: cannot open file\n", 25);
        return 1;
    }

    // Get file size
    struct stat st;
    if (sys_fstat(fd, &st) != 0) {
        sys_write(2, "Error: fstat failed\n", 20);
        sys_close(fd);
        return 1;
    }
    off_t file_size = st.st_size;

    if (file_size == 0) {
        sys_close(fd);
        return 0;
    }

    // Your solution here!

    sys_close(fd);
    return found_any ? 0 : 1;
}