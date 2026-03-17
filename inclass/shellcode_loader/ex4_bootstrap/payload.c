/*
 * Exercise 4 — Payload SO for bootstrap loading
 *
 * This SO is loaded by the reflective memfd loader (loader.c).
 * Its constructor fires automatically when dlopen is called.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

__attribute__((constructor))
static void payload_init(void)
{
    printf("[payload] constructor fired from bootstrap chain!\n");
    printf("[payload] PID = %d\n", getpid());
}
