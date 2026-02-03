#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

int main(void)
{
	printf("ex4: Symbol Resolution (RTLD_DEFAULT / RTLD_NEXT)\n\n");

	/* Use RTLD_DEFAULT to find printf in the global symbol table */
	int (*my_printf)(const char *, ...) = dlsym(RTLD_DEFAULT, "printf");
	if (!my_printf) {
		fprintf(stderr, "dlsym(RTLD_DEFAULT, \"printf\") failed: %s\n",
			dlerror());
		return 1;
	}

	printf("Found printf via RTLD_DEFAULT at address: %p\n", (void *)my_printf);
	my_printf("Calling printf through function pointer: it works!\n");

	/*
	 * puts() is intercepted by wraplib.so (linked at compile time).
	 * wraplib's puts() uses RTLD_NEXT to call libc's real puts().
	 */
	printf("\nputs() is intercepted by wraplib.so via RTLD_NEXT:\n");
	puts("Hello from main!");

	return 0;
}
