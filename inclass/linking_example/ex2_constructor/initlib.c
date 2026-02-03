#include <stdio.h>

__attribute__((constructor))
static void lib_init(void)
{
	printf("[initlib] constructor: library loaded!\n");
}

__attribute__((destructor))
static void lib_fini(void)
{
	printf("[initlib] destructor: library unloading!\n");
}

void do_something(void)
{
	printf("[initlib] do_something() called\n");
}
