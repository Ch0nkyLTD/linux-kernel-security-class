/*
 * Exercise 2 — /dev/shm + dlopen
 *
 * Identical to Exercise 1, but writes to /dev/shm instead of /tmp.
 * One line changes.  Observe: the file is still visible via ls /dev/shm
 * during the window between open() and unlink().
 *
 * Build:  make ex2
 * Run:    make run-ex2
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "payload_bytes.h"

int main(void)
{
    /* ── TODO: change the path to /dev/shm/.payload.so ─────── */
    const char *path = "/tmp/payload.so";  /* <-- fix this */

    printf("[ex2] Writing %u bytes to %s\n",
           payload_bytes_len, path);

    int fd = open(path, O_CREAT | O_WRONLY, 0755);
    if (fd < 0) { perror("open"); return 1; }

    write(fd, payload_bytes, payload_bytes_len);
    close(fd);

    void *handle = dlopen(path, RTLD_NOW);
    if (!handle)
        fprintf(stderr, "dlopen: %s\n", dlerror());

    unlink(path);
    if (handle) dlclose(handle);

    printf("[ex2] Done.\n");
    return 0;
}
