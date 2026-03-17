/*
 * Exercise 3a Solution — Find libc base address (C prototype)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(void)
{
    FILE *fp = fopen("/proc/self/maps", "r");
    if (!fp) { perror("fopen"); return 1; }

    char line[512];
    uintptr_t libc_base = 0;
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "libc") && strstr(line, "r-xp")) {
            libc_base = strtoul(line, NULL, 16);
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (found) {
        printf("[find_libc] libc base: 0x%lx\n", (unsigned long)libc_base);
    } else {
        printf("[find_libc] libc not found in /proc/self/maps\n");
        return 1;
    }

    return 0;
}
