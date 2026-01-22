#include "syscall_utils.h"
#include "minc.h"

int main(int argc, char **argv, char **envp) {
  const char hello[] = "hello world!\n";
  sys_write(1, hello, sizeof(hello));

  for (int i = 0; i < argc; i++) {

    char *s = argv[i];
    unsigned long len = 0;
    while (s[len])
      len++;

    sys_write(1, s, len);
    sys_write(1, "\n", 1);
  }

  return 0;
}
