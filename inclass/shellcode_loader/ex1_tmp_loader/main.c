/*
 * Exercise 1 — /tmp + dlopen
 *
 * Load a shared object by writing it to /tmp, calling dlopen(),
 * then cleaning up with unlink().
 *
 * The payload SO bytes are embedded via xxd -i (see inc/payload_bytes.h).
 *
 * Build:  make ex1
 * Run:    make run-ex1
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

    /* ── TODO 1: open the file for writing ─────────────────────
     *
     * Use open() with O_CREAT | O_WRONLY and mode 0755.
     * Store the file descriptor in `fd`.
     */
    int fd = -1;
    // fd = open( ... );

    /* ── TODO 2: write the payload bytes ───────────────────────
     *
     * Write payload_bytes (payload_bytes_len bytes) into fd.
     * Then close fd.
     */
    // write( ... );
    // close( ... );

    /* ── TODO 3: dlopen the shared object ──────────────────────
     *
     * Call dlopen(path, RTLD_NOW).  The constructor in initlib.c
     * fires automatically.  Print dlerror() on failure.
     */
    void *handle = NULL;
    // handle = dlopen( ... );

    /* ── TODO 4: clean up ──────────────────────────────────────
     *
     * unlink() the file so it disappears from /tmp.
     * dlclose() the handle if it was opened.
     */
    // unlink( ... );

    printf("[ex1] Done.\n");
    return 0;
}
