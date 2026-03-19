/*
 * procfsutil.h — Find a library's base address via /proc/self/maps
 *
 * No-libc version: uses raw syscalls and stack strings only.
 */
#ifndef PROCFSUTIL_H
#define PROCFSUTIL_H

#include "minstr.h"
#include "syscall.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>

/* Build "/proc/self/maps" on the stack so it never appears in .rodata */
#define STACK_STR_PROC_SELF_MAPS(var)                                         \
  char var[16];                                                               \
  var[0] = '/';  var[1] = 'p';  var[2] = 'r';  var[3] = 'o';                \
  var[4] = 'c';  var[5] = '/';  var[6] = 's';  var[7] = 'e';                \
  var[8] = 'l';  var[9] = 'f';  var[10] = '/'; var[11] = 'm';               \
  var[12] = 'a'; var[13] = 'p'; var[14] = 's'; var[15] = '\0';

/*
 * search_lib_procmaps — find a library's base address
 *
 * Reads /proc/self/maps, finds the first line matching both
 * lib_needle (e.g. "libc.so") and prot (e.g. "r-xp"), then
 * parses the hex start address.
 *
 * @lib_needle: substring to match in the pathname column
 * @prot:       permission string to match (e.g. "r-xp")
 * @base_out:   receives the parsed base address on success
 *
 * Returns 0 on success, -1 on failure.
 */
static int search_lib_procmaps(char *lib_needle, char *prot,
                               uintptr_t *base_out)
{
    char buf[4096];
    int fd = -1;
    int bytes_read = 0;

    /* ── Step 1: Open /proc/self/maps ──────────────────────────
     *
     * Build the path on the stack (use STACK_STR_PROC_SELF_MAPS).
     * Open with sys_openat(AT_FDCWD, path, O_RDONLY, 0).
     * Return -1 on failure.
     */
    // TODO: Your implementation here

    /* ── Step 2: Read the file into buf ────────────────────────
     *
     * sys_read(fd, buf, sizeof(buf) - 1)
     * Null-terminate: buf[bytes_read] = '\0'
     * Close the fd.
     */
    // TODO: Your implementation here

    /* ── Step 3: Parse line by line ────────────────────────────
     *
     * Walk buf character by character. At each '\n':
     *   - Null-terminate the line temporarily
     *   - Find the first space (end of address range)
     *   - Check if permissions match (memcmp with prot)
     *   - Skip to the pathname (after 5th space)
     *   - Check if lib_needle is in the pathname (strstr)
     *   - Parse the hex address with hex_str_to_u64()
     *   - Store in *base_out and return 0
     *
     * Restore the '\n' before continuing to the next line.
     */
    // TODO: Your implementation here

    return -1;
}

#endif /* PROCFSUTIL_H */
