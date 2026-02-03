#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

int puts(const char *s)
{
	/* Get the real puts -- do NOT call puts() here or infinite recursion! */
	int (*real_puts)(const char *) = dlsym(RTLD_NEXT, "puts");

	real_puts("I WAS HERE FIRST!");
	return real_puts(s);
}
