/* _start for -nostdlib builds: call loader_main(), then exit */
    .arch armv8-a
    .text
    .global _start

_start:
    bl  loader_main
    mov x8, #93            /* SYS_exit */
    mov x0, #0
    svc #0
