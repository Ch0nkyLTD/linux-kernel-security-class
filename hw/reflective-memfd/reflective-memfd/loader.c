/*
 * loader.c — Reflective memfd loader
 *
 * Compiled as a -nostdlib -fPIC shared object. A bootstrap stub
 * branches into load_memfd() with the SO size in x0.
 *
 * Flow: find own base → find libc → resolve dlopen/sprintf →
 *       create memfd → write self → dlopen(/proc/self/fd/N) →
 *       constructor fires.
 */
#define _GNU_SOURCE
#include "procfsutil.h"
#include "elfsym.h"
#include "syscall.h"
#include "minstr.h"

#include <dlfcn.h>
#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>

/* Stack string macros — avoid .rodata leaks */
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

/*
 * find_base — find our own ELF base by scanning backwards
 *
 * We know the code is page-aligned and starts with \x7FELF.
 * Take any known address in our SO, page-align it, then scan
 * backwards page by page looking for the ELF magic.
 */
static uint8_t *find_base(void)
{
    // TODO: Your implementation here
    return NULL;
}

/*
 * load_memfd — entry point called by bootstrap stub
 *
 * @span: total size of the SO file (passed in x0 by bootstrap)
 *
 * Steps:
 *   1. Find our own ELF base address
 *   2. Create an anonymous memfd
 *   3. Find libc in /proc/self/maps, resolve dlopen + sprintf
 *   4. Write ourselves to the memfd
 *   5. dlopen("/proc/self/fd/<N>") — triggers constructor
 */
void *load_memfd(size_t span)
{
    msg("[bootstrap] reflective loader started\n");

    uint8_t *base = find_base();
    if (!base) { msg("[-] cannot find own ELF base\n"); return NULL; }

    msg("[bootstrap] found ELF base\n");

    /* Step 2: Create anonymous file via memfd_create */
    // TODO: Your implementation here

    /* Step 3: Find libc and resolve dlopen + sprintf */
    // TODO: Your implementation here

    /* Step 4-5: Write SO to memfd and dlopen it */
    // TODO: Your implementation here
    return NULL;
}

void dummy(void) {}
