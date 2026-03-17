/*
 * Exercise 3 — No-libc loader
 *
 * Loads a shared object via memfd_create + dlopen, but resolves
 * dlopen at runtime by parsing /proc/self/maps and walking libc's
 * ELF symbol table.  Compiled with -nostdlib as a shared object.
 *
 * The tester binary (tester.c) guarantees libc is mapped in the
 * process.  This SO is loaded via LD_PRELOAD; its constructor
 * runs automatically.
 *
 * Build:  make ex3
 * Run:    make run-ex3
 */
#include "syscall.h"
#include "minstr.h"
#include "procfsutil.h"
#include "elfsym.h"
#include "payload_bytes.h"

#include <elf.h>
#include <fcntl.h>
#include <stdint.h>

/* RTLD_NOW = 0x2 on AArch64 Linux */
#ifndef RTLD_NOW
#define RTLD_NOW 0x00002
#endif

/* ── Stack string macros ──────────────────────────────────────── */

#define STACK_STR_LIBCSO(var)                                                 \
  char var[8];                                                                \
  var[0]='l'; var[1]='i'; var[2]='b'; var[3]='c';                            \
  var[4]='.'; var[5]='s'; var[6]='o'; var[7]='\0';

#define STACK_STR_RXP(var)                                                    \
  char var[5];                                                                \
  var[0]='r'; var[1]='-'; var[2]='x'; var[3]='p'; var[4]='\0';

#define STACK_STR_DLOPEN(var)                                                 \
  char var[7];                                                                \
  var[0]='d'; var[1]='l'; var[2]='o'; var[3]='p';                            \
  var[4]='e'; var[5]='n'; var[6]='\0';

#define STACK_STR_SPRINTF(var)                                                \
  char var[8];                                                                \
  var[0]='s'; var[1]='p'; var[2]='r'; var[3]='i';                            \
  var[4]='n'; var[5]='t'; var[6]='f'; var[7]='\0';

/* Function pointer types */
typedef void *(*dlopen_func_t)(const char *filename, int flag);
typedef int (*sprintf_func_t)(char *str, const char *format, ...);

/* Simple write-to-stderr for debug output (no libc) */
static void msg(const char *s)
{
    sys_write(2, s, strlen(s));
}

/* ── Main loader logic (called as constructor via LD_PRELOAD) ── */
__attribute__((constructor))
void loader_main(void)
{
    msg("[ex3] no-libc loader starting\n");

    /* ── TODO 1: find libc base address ───────────────────────
     *
     * Build stack strings for "libc.so" and "r-xp".
     * Call search_lib_procmaps(libc_str, perm, &libc_base).
     * Print an error and return on failure.
     */
    uintptr_t libc_base = 0;
    // STACK_STR_LIBCSO(libc_str);
    // STACK_STR_RXP(perm);
    // if (search_lib_procmaps( ... ) != 0) { msg("failed\n"); return; }

    /* ── TODO 2: resolve dlopen and sprintf ───────────────────
     *
     * Build stack strings for "dlopen" and "sprintf".
     * Call find_symbol_symtab((void *)libc_base, symbol_name)
     * for each.  Cast results to dlopen_func_t and sprintf_func_t.
     */
    dlopen_func_t  g_dlopen  = NULL;
    sprintf_func_t g_sprintf = NULL;
    // STACK_STR_DLOPEN(str_dlopen);
    // STACK_STR_SPRINTF(str_sprintf);
    // g_dlopen  = (dlopen_func_t)find_symbol_symtab( ... );
    // g_sprintf = (sprintf_func_t)find_symbol_symtab( ... );

    if (!g_dlopen) { msg("[-] dlopen not found\n"); return; }
    if (!g_sprintf) { msg("[-] sprintf not found\n"); return; }

    msg("[ex3] resolved dlopen + sprintf\n");

    /* ── TODO 3: create memfd and write SO bytes ──────────────
     *
     * int mem_fd = sys_memfd_create("x", MFD_CLOEXEC);
     * sys_write(mem_fd, payload_bytes, payload_bytes_len);
     */
    int mem_fd = -1;
    // mem_fd = sys_memfd_create( ... );
    // sys_write( ... );

    /* ── TODO 4: build /proc/self/fd/<N> path and dlopen ──────
     *
     * Use g_sprintf to format "/proc/self/fd/%d" with mem_fd.
     * Build the format string on the stack (stack string).
     * Call g_dlopen(path, RTLD_NOW) — constructor fires.
     */
    // char proc_path[64];
    // ...format string on stack...
    // g_sprintf(proc_path, fmt, mem_fd);
    // g_dlopen(proc_path, RTLD_NOW);

    msg("[ex3] done\n");
}
