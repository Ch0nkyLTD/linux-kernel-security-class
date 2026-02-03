#include <stdio.h>
#include "mylib.h"

void greet(const char *name)
{
	printf("Hello, %s! Greetings from libmylib.so\n", name);
}
