/*
 * payload.c — demo payload
 *
 * Writes "foobar\n" to ./pwned.txt and exits.
 * Built twice: native (for make demo) and AArch64 (for QEMU lab).
 */

#include <stdio.h>

int main(void) {
    FILE *f = fopen("./pwned.txt", "w");
    if (f) {
        fputs("foobar\n", f);
        fclose(f);
    }
    return 0;
}
