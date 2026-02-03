#include <stdio.h>

int square(int x)
{
	printf("[mathlib] computing square(%d)\n", x);
	return x * x;
}

int cube(int x)
{
	printf("[mathlib] computing cube(%d)\n", x);
	return x * x * x;
}
