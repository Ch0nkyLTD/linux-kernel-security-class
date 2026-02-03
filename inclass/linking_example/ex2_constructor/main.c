#include <stdio.h>

extern void do_something(void);

int main(void)
{
	printf("ex2: Constructor / Destructor\n");
	printf("[main] inside main()\n");
	do_something();
	printf("[main] leaving main()\n");
	return 0;
}
