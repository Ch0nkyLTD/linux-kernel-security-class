/* bgrep.c - Binary grep using direct syscalls */
#include "syscall_utils.h"
#include "minc.h"

#define CHUNK_SIZE 4096
#define MAX_PATTERN_SIZE 256

    // Your solution here!

int main(int argc, char **argv) {
  if (argc != 3) {
    sys_write(2, "Usage: bgrep <file> <hexpattern>\n", 33);
    return 1;
  }

  const char *filename = argv[1];
  const char *hex_pattern = argv[2];

    // Your solution here!
  return found_any ? 0 : 1;
}
