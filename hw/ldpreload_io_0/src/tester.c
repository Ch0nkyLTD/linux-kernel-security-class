#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define TARGET_DIR "/tmp/protected"
#define TARGET_FILE "/tmp/protected/secret.txt"
#define DATA_TO_WRITE "This is secret data.\n"

void print_status(const char *action, int result, int err_code) {
    if (result >= 0) {
        printf("    [OPEN] %s: Success.\n", action);
    } else {
        if (err_code == EACCES || err_code == EPERM) {
            printf("    [BLOCK] %s: Blocked by Rootkit! (Error: %s)\n", action, strerror(err_code));
        } else {
            printf("    [FAIL] %s: Failed (Error: %s)\n", action, strerror(err_code));
        }
    }
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    int fd;
    ssize_t bytes;
    int ret;

    printf("==============================================\n");
    printf("   LD_PRELOAD ROOTKIT: FUNCTIONALITY TESTER   \n");
    printf("==============================================\n");
    printf("Target: %s\n", TARGET_FILE);
    printf("My PID: %d | My GID: %d\n", getpid(), getgid());
    printf("----------------------------------------------\n");

    /* Ensure the protected directory exists (not blocked by the hooks). */
    if (mkdir(TARGET_DIR, 0755) < 0 && errno != EEXIST) {
        printf("    [WARN] mkdir(%s) failed: %s\n", TARGET_DIR, strerror(errno));
    }

    /* TEST 1: CREATE / WRITE */
    printf("[*] Attempting to CREATE/WRITE file...\n");
    fd = openat(AT_FDCWD, TARGET_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    print_status("open(O_WRONLY)", fd, errno);

    if (fd >= 0) {
        bytes = write(fd, DATA_TO_WRITE, strlen(DATA_TO_WRITE));
        if (bytes < 0) {
            print_status("write()", -1, errno);
        } else {
            printf("    [OPEN] write(): wrote %ld bytes.\n", bytes);
        }
        close(fd);
    }

    printf("----------------------------------------------\n");

    /* TEST 2: READ */
    printf("[*] Attempting to READ file...\n");
    fd = openat(AT_FDCWD, TARGET_FILE, O_RDONLY);
    print_status("open(O_RDONLY)", fd, errno);

    if (fd >= 0) {
        char buf[100];
        bytes = read(fd, buf, sizeof(buf)-1);
        if (bytes < 0) {
            print_status("read()", -1, errno);
        } else {
            buf[bytes] = '\0';
            printf("    [OPEN] read(): Content -> \"%s\"\n", buf); // Truncated for display
        }
        close(fd);
    }

    printf("----------------------------------------------\n");

    /* TEST 3: DELETE */
    printf("[*] Attempting to DELETE (unlink) file...\n");
    ret = unlinkat(AT_FDCWD, TARGET_FILE, 0);
    print_status("unlinkat()", ret, errno);

    printf("==============================================\n");
    
    // Return 0 if at least one operation was blocked (Rootkit Active default state)
    // Return 1 if everything succeeded (God Mode / No Rootkit)
    if (ret == 0) return 1; 
    return 0;
}
