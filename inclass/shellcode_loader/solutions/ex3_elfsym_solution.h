/*
 * Exercise 3 Solution — elfsym.h (no-libc ELF symbol walker)
 */
#ifndef ELFSYM_H
#define ELFSYM_H

#include "minstr.h"
#include <elf.h>
#include <stddef.h>
#include <stdint.h>

static void *find_symbol_symtab(void *base_addr, char *symbol_name)
{
    unsigned char *base = (unsigned char *)base_addr;
    if (!base || base == (void *)-1)
        return NULL;

    if (memcmp(base, ELFMAG, SELFMAG) != 0)
        return NULL;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)base;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(base + ehdr->e_phoff);

    Elf64_Dyn *dynamic_section = NULL;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_DYNAMIC) {
            dynamic_section = (Elf64_Dyn *)(base + phdr[i].p_vaddr);
            break;
        }
    }
    if (!dynamic_section)
        return NULL;

    char *strtab = NULL;
    Elf64_Sym *symtab = NULL;
    size_t strtab_size = 0;

    for (Elf64_Dyn *dyn = dynamic_section; dyn->d_tag != DT_NULL; dyn++) {
        switch (dyn->d_tag) {
        case DT_STRTAB:  strtab      = (char *)(dyn->d_un.d_ptr);       break;
        case DT_SYMTAB:  symtab      = (Elf64_Sym *)(dyn->d_un.d_ptr);  break;
        case DT_STRSZ:   strtab_size = dyn->d_un.d_val;                 break;
        }
    }

    if (!strtab || !symtab)
        return NULL;

    for (size_t i = 0; ; i++) {
        Elf64_Sym *sym = &symtab[i];
        if (sym->st_name >= strtab_size)
            break;
        const char *name = strtab + sym->st_name;
        if (strcmp(name, symbol_name) == 0)
            return (void *)((uintptr_t)base_addr + sym->st_value);
    }

    return NULL;
}

#endif /* ELFSYM_H */
