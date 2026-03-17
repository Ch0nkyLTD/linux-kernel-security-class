/*
 * elfsym.h — Find a symbol in a mapped ELF's dynamic symbol table
 *
 * No-libc version: uses our strcmp() from minstr.h.
 * Same algorithm as find_symbol.c — just pointer arithmetic on
 * the already-mapped ELF.  No libc calls needed.
 */
#ifndef ELFSYM_H
#define ELFSYM_H

#include "minstr.h"
#include <elf.h>
#include <stddef.h>
#include <stdint.h>

/*
 * find_symbol_symtab — resolve a symbol from a mapped ELF
 *
 * @base_addr:    base address of the mapped shared object
 * @symbol_name:  name to find (e.g. "dlopen")
 *
 * Returns the symbol's absolute address, or NULL on failure.
 *
 * Walk: Ehdr → Phdr → PT_DYNAMIC → DT_SYMTAB + DT_STRTAB →
 *       iterate → strcmp → base + st_value
 */
static void *find_symbol_symtab(void *base_addr, char *symbol_name)
{
    unsigned char *base = (unsigned char *)base_addr;
    if (!base || base == (void *)-1)
        return NULL;

    /* ── TODO 1: verify ELF magic ─────────────────────────────
     *
     * if (memcmp(base, ELFMAG, SELFMAG) != 0) return NULL;
     */

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)base;

    /* ── TODO 2: walk program headers to find PT_DYNAMIC ──────
     *
     * Elf64_Phdr *phdr = (Elf64_Phdr *)(base + ehdr->e_phoff);
     * for i in 0..e_phnum: if p_type == PT_DYNAMIC ...
     * dynamic_section = (Elf64_Dyn *)(base + phdr[i].p_vaddr);
     */
    Elf64_Dyn *dynamic_section = NULL;
    // ...

    if (!dynamic_section)
        return NULL;

    /* ── TODO 3: extract DT_STRTAB, DT_SYMTAB, DT_STRSZ ─────
     *
     * Walk dynamic entries (dyn->d_tag != DT_NULL):
     *   DT_STRTAB → strtab (absolute address, cast to char *)
     *   DT_SYMTAB → symtab (absolute address, cast to Elf64_Sym *)
     *   DT_STRSZ  → strtab_size
     */
    char *strtab = NULL;
    Elf64_Sym *symtab = NULL;
    size_t strtab_size = 0;
    // ...

    if (!strtab || !symtab)
        return NULL;

    /* ── TODO 4: iterate symbol table, find match ─────────────
     *
     * for (i = 0; ; i++):
     *   if symtab[i].st_name >= strtab_size: break
     *   if strcmp(strtab + sym->st_name, symbol_name) == 0:
     *       return (void *)((uintptr_t)base_addr + sym->st_value)
     */

    return NULL;
}

#endif /* ELFSYM_H */
