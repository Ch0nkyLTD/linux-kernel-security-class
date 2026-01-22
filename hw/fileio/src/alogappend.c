/* alogappend.c - Append entries to structured log using direct syscalls */
#include "minc.h"
#include "syscall_utils.h"

#pragma pack(1)
struct log_entry {
  uint64_t id;
  uint64_t timestamp;
  uint16_t msglen;
  uint8_t padding[6];
  char message[240];
};

//  Simulare timestamp
static uint64_t get_current_timestamp(void) {
  static uint64_t counter = 1000000000; // Start from a large value
  return counter++;
}

    // Your solution here!

int main(int argc, char **argv) {
  if (argc != 4) {
    const char *usage = "Usage: alogappend <logfile> <id> <message>\n";
    sys_write(2, usage, strlen(usage));
    return 1;
  }

  const char *logfile = argv[1];
  const char *id_str = argv[2];
  const char *message = argv[3];

    // Your solution here!
  return 0;
}
