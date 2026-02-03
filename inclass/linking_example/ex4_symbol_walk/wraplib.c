#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

/*
 * Override puts: print a tag line, then forward to the real puts.
 *
 * This works because wraplib.so is linked at compile time, placing it
 * BEFORE libc in the link map.  RTLD_NEXT therefore finds libc's puts.
 */
int puts(const char *s)
{
	int (*real_puts)(const char *) = dlsym(RTLD_NEXT, "puts");
	if (!real_puts) {
		/* Can't use puts (that's us) or printf (might also be wrapped). */
		fputs("[wraplib] dlsym(RTLD_NEXT, \"puts\") failed!\n", stderr);
		return EOF;
	}

	real_puts("[wraplib] intercepted puts:");
	return real_puts(s);
}
