/* timestomp-lite.c - File timestamp modification using direct syscalls */
#include "minc.h"
#include "syscall_utils.h"

    // Your solution here!

int main(int argc, char **argv) {
  if (argc < 3) {
    const char *usage =
        "Usage: timestomp-lite <file> --at <epoch> --mt <epoch>\n"
        "       timestomp-lite <file> --copy-from <otherfile>\n";
    sys_write(2, usage, strlen(usage));
    return 1;
  }

  const char *filename = argv[1];

    // Your solution here!
  return 1;
}
