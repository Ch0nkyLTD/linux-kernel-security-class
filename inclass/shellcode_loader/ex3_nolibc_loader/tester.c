/*
 * Tester binary for Exercise 3.
 *
 * This simple program guarantees libc is loaded in the process.
 * Run with LD_PRELOAD=bin/ex3.so to trigger the no-libc loader
 * via its constructor.
 *
 * Usage:  LD_PRELOAD=./bin/ex3.so ./bin/ex3_tester
 */
#include <stdio.h>

int main(void)
{
    printf("[tester] libc is loaded.  If you see this, LD_PRELOAD worked.\n");
    return 0;
}
