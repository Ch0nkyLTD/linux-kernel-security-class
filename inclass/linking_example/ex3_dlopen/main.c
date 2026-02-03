#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(void)
{
	printf("ex3: Explicit Dynamic Linking (dlopen / dlsym)\n");

	void *handle = dlopen("./bin/libmathlib.so", RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "dlopen failed: %s\n", dlerror());
		return 1;
	}

	/* clear any existing error */
	dlerror();

	int (*square)(int) = dlsym(handle, "square");
	char *err = dlerror();
	if (err) {
		fprintf(stderr, "dlsym(square) failed: %s\n", err);
		dlclose(handle);
		return 1;
	}

	int (*cube)(int) = dlsym(handle, "cube");
	err = dlerror();
	if (err) {
		fprintf(stderr, "dlsym(cube) failed: %s\n", err);
		dlclose(handle);
		return 1;
	}

	int val = 7;
	printf("square(%d) = %d\n", val, square(val));
	printf("cube(%d)   = %d\n", val, cube(val));

	dlclose(handle);
	printf("Library closed.\n");
	return 0;
}
