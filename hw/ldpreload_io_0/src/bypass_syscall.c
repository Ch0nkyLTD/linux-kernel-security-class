/* bypass_syscall.c - Direct syscalls to write protected file */
#include "minc.h"
#include "syscall_utils.h"

#define TARGET_FILE "/tmp/protected/hacked.txt"
#define PAYLOAD "1337h4x0r"

static void write_msg(int fd, const char *msg) {
    sys_write(fd, msg, strlen(msg));
}

static int write_payload(void) {
    int status = 1;
    // Your solution here!
    return status;
}

int main(int argc, char **argv, char **envp) {
    (void)argc;
    (void)argv;
    (void)envp;

    return write_payload();
}
