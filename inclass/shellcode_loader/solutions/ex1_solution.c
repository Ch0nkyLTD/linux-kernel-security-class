/*
 * Exercise 1 Solution — /tmp + dlopen
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "payload_bytes.h"

int main(void)
{
    const char *path = "/tmp/payload.so";

    printf("[ex1] Writing %u bytes to %s\n",
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

    printf("[ex1] Done.\n");
    return 0;
}
