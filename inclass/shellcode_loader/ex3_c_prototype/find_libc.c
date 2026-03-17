/*
 * Exercise 3a — Find libc base address (C prototype)
 *
 * Parse /proc/self/maps to find the base address of libc.
 * This is the NORMAL C version using libc (fopen, fgets, etc.).
 * Once this works, you'll convert the algorithm to the no-libc
 * "reflective dialect" in ex3_nolibc_loader/procfsutil.h.
 *
 * Build:  make ex3-proto
 * Run:    make run-ex3-proto-libc
 *
 * /proc/self/maps format:
 *   address           perms offset  dev   inode   pathname
 *   7f8a00000-7f8a01000 r-xp 00000000 08:02 173521 /usr/lib/libc.so.6
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(void)
{
    /* ── TODO 1: open /proc/self/maps ──────────────────────────
     *
     * Use fopen() with mode "r".
     */
    FILE *fp = NULL;
    // fp = fopen( ... );

    char line[512];
    uintptr_t libc_base = 0;
    int found = 0;

    /* ── TODO 2: read lines and search for libc ────────────────
     *
     * For each line (fgets):
     *   1. Check if it contains "libc" (strstr)
     *   2. Check if it contains "r-xp" (executable segment)
     *   3. Parse the hex address before the first '-'
     *      (strtoul with base 16)
     *   4. Store in libc_base, set found = 1, break
     */
    // while (fgets(line, sizeof(line), fp)) {
    //     ...
    // }

    if (fp) fclose(fp);

    if (found) {
        printf("[find_libc] libc base: 0x%lx\n", (unsigned long)libc_base);
    } else {
        printf("[find_libc] libc not found in /proc/self/maps\n");
        return 1;
    }

    return 0;
}
