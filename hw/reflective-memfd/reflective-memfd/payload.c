/*
 * payload.c — Payload SO for reflective memfd loading
 *
 * This SO is loaded by the reflective loader (loader.c) via
 * memfd_create + dlopen. Its constructor fires automatically
 * and creates a proof file that the test harness checks.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

__attribute__((constructor))
static void payload_init(void)
{
    printf("[payload] constructor fired from bootstrap chain!\n");
    printf("[payload] PID = %d\n", getpid());

    /* Create proof file for the test harness */
    char path[64];
    snprintf(path, sizeof(path), "/tmp/payload_loaded_%d", getpid());

    FILE *f = fopen(path, "w");
    if (f) {
        fprintf(f, "PAYLOAD_OK\n");
        fclose(f);
        printf("[payload] proof file created: %s\n", path);
    } else {
        printf("[payload] WARNING: could not create proof file\n");
    }
}
