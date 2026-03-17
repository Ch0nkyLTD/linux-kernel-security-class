/*
 * Exercise 3b — Find a symbol in a mapped ELF (C prototype)
 *
 * Given libc's base address in memory, walk the ELF structures
 * to find a symbol by name.  This is the NORMAL C version —
 * we use dlopen/dlsym to verify our answer.
 *
 * Build:  make ex3-proto
 * Run:    make run-ex3-proto-sym
 *
 * ELF pointer chain:
 *   Ehdr → Phdr array → find PT_DYNAMIC →
 *   DT_SYMTAB + DT_STRTAB + DT_STRSZ →
 *   iterate symbols → strcmp(name) → base + st_value
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * find_symbol_symtab — walk ELF dynamic symbol table
 *
 * @base_addr:    base address of the mapped ELF (e.g., libc)
 * @symbol_name:  name to search for (e.g., "dlopen")
 *
 * Returns the resolved address, or NULL on failure.
 */
static void *find_symbol_symtab(void *base_addr, const char *symbol_name)
{
    unsigned char *base = (unsigned char *)base_addr;

    /* ── TODO 1: verify ELF magic ─────────────────────────────
     *
     * Check that the first 4 bytes are 0x7f 'E' 'L' 'F'.
     * Use memcmp(base, ELFMAG, SELFMAG).
     * Return NULL if not an ELF.
     */

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)base;

    /* ── TODO 2: find PT_DYNAMIC ──────────────────────────────
     *
     * Walk the program header array:
     *   Elf64_Phdr *phdr = (Elf64_Phdr *)(base + ehdr->e_phoff);
     *   for i in 0..e_phnum: if phdr[i].p_type == PT_DYNAMIC ...
     * Compute dynamic section address: base + phdr[i].p_vaddr
     */
    Elf64_Dyn *dynamic_section = NULL;
    // ...

    if (!dynamic_section) {
        fprintf(stderr, "  [-] PT_DYNAMIC not found\n");
        return NULL;
    }

    /* ── TODO 3: parse DT_SYMTAB, DT_STRTAB, DT_STRSZ ───────
     *
     * Walk dynamic entries until DT_NULL:
     *   DT_STRTAB  → char *strtab  (absolute address)
     *   DT_SYMTAB  → Elf64_Sym *symtab (absolute address)
     *   DT_STRSZ   → size_t strtab_size
     */
    char *strtab = NULL;
    Elf64_Sym *symtab = NULL;
    size_t strtab_size = 0;
    // ...

    if (!strtab || !symtab) {
        fprintf(stderr, "  [-] missing strtab or symtab\n");
        return NULL;
    }

    /* ── TODO 4: iterate symbols ──────────────────────────────
     *
     * for (i = 0; ; i++):
     *   sym = &symtab[i]
     *   if sym->st_name >= strtab_size: break (end of table)
     *   name = strtab + sym->st_name
     *   if strcmp(name, symbol_name) == 0:
     *       return (void *)((uintptr_t)base_addr + sym->st_value)
     */
    // ...

    fprintf(stderr, "  [-] symbol '%s' not found\n", symbol_name);
    return NULL;
}

int main(void)
{
    /* Use dlopen to get libc's handle, then dlinfo to get base.
     * We'll compare our manual result against dlsym's result.
     */
    void *libc_handle = dlopen("libc.so.6", RTLD_NOW | RTLD_NOLOAD);
    if (!libc_handle) {
        fprintf(stderr, "dlopen(libc): %s\n", dlerror());
        return 1;
    }

    /* Get libc base by reading /proc/self/maps (reuse find_libc logic) */
    FILE *fp = fopen("/proc/self/maps", "r");
    char line[512];
    uintptr_t libc_base = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "libc") && strstr(line, "r-xp")) {
            libc_base = strtoul(line, NULL, 16);
            break;
        }
    }
    fclose(fp);

    if (!libc_base) {
        fprintf(stderr, "Could not find libc base\n");
        return 1;
    }
    printf("[find_symbol] libc base: 0x%lx\n", (unsigned long)libc_base);

    /* Test: find "dlopen" manually, compare with dlsym */
    const char *target = "dlopen";
    void *manual = find_symbol_symtab((void *)libc_base, target);
    void *expected = dlsym(libc_handle, target);

    printf("[find_symbol] manual  %s @ %p\n", target, manual);
    printf("[find_symbol] dlsym   %s @ %p\n", target, expected);

    if (manual == expected)
        printf("[find_symbol] MATCH!\n");
    else
        printf("[find_symbol] MISMATCH -- check your code\n");

    dlclose(libc_handle);
    return 0;
}
