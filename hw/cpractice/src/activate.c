#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define KEY "/tmp/ch0nky.txt"

int activate() {
  printf("IDK ...\n");
  // Your solution here!
  return 0;
}

int deactivate() {
  // Your solution here!
  return 0;
}

void usage(char *argv_0) { printf("Usage: %s <activate|deactivate>", argv_0); }

int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
    return 0;
  }
  if (strcmp(argv[1], "activate") == 0) {
    activate();
  } else if (strcmp(argv[1], "deactivate") == 0) {
    deactivate();
  } else {
    usage(argv[0]);
  }

  return 0;
}
