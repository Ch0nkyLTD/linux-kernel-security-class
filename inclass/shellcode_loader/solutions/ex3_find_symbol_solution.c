/*
 * Exercise 3b Solution — Find a symbol in a mapped ELF (C prototype)
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

static void *find_symbol_symtab(void *base_addr, const char *symbol_name)
{
    unsigned char *base = (unsigned char *)base_addr;

    if (memcmp(base, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "  [-] not a valid ELF\n");
        return NULL;
    }

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)base;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(base + ehdr->e_phoff);

    Elf64_Dyn *dynamic_section = NULL;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_DYNAMIC) {
            dynamic_section = (Elf64_Dyn *)(base + phdr[i].p_vaddr);
            break;
        }
    }
    if (!dynamic_section) {
        fprintf(stderr, "  [-] PT_DYNAMIC not found\n");
        return NULL;
    }

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

    if (!strtab || !symtab) {
        fprintf(stderr, "  [-] missing strtab or symtab\n");
        return NULL;
    }

    for (size_t i = 0; ; i++) {
        Elf64_Sym *sym = &symtab[i];
        if (sym->st_name >= strtab_size)
            break;
        if (strcmp(strtab + sym->st_name, symbol_name) == 0)
            return (void *)((uintptr_t)base_addr + sym->st_value);
    }

    fprintf(stderr, "  [-] symbol '%s' not found\n", symbol_name);
    return NULL;
}

int main(void)
{
    void *libc_handle = dlopen("libc.so.6", RTLD_NOW | RTLD_NOLOAD);
    if (!libc_handle) {
        fprintf(stderr, "dlopen(libc): %s\n", dlerror());
        return 1;
    }

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

    const char *target = "dlopen";
    void *manual   = find_symbol_symtab((void *)libc_base, target);
    void *expected = dlsym(libc_handle, target);

    printf("[find_symbol] manual  %s @ %p\n", target, manual);
    printf("[find_symbol] dlsym   %s @ %p\n", target, expected);

    if (manual == expected)
        printf("[find_symbol] MATCH!\n");
    else
        printf("[find_symbol] MISMATCH\n");

    dlclose(libc_handle);
    return 0;
}
