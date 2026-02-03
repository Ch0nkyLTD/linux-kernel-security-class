/* binding_hook.c - LD_PRELOAD passive shim for protected files */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* --- PART 1: FUNCTION SIGNATURES --- */
typedef int (*orig_open_type)(const char *pathname, int flags, mode_t mode);
typedef int (*orig_open64_type)(const char *pathname, int flags, mode_t mode);
typedef int (*orig_openat_type)(int dirfd, const char *pathname, int flags,
                                mode_t mode);
typedef int (*orig_unlink_type)(const char *pathname);
typedef int (*orig_unlinkat_type)(int dirfd, const char *pathname, int flags);

/* Global pointers to real functions */
static orig_open_type real_open = NULL;
static orig_open64_type real_open64 = NULL;
static orig_openat_type real_openat = NULL;
static orig_unlink_type real_unlink = NULL;
static orig_unlinkat_type real_unlinkat = NULL;

/* --- PART 2: THE CONSTRUCTOR (INSTALLER) --- */
__attribute__((constructor)) void install_hooks(void) {
    // Your solution here!
}

/* --- PART 3: HELPER LOGIC --- */
#define PROTECTED_DIR "/tmp/protected"
static int is_protected(const char *path) {
    int blocked = 0;
    // Your solution here!
    return blocked;
}

/* --- PART 4: THE HOOKS (DETOURS) --- */

int open(const char *pathname, int flags, ...) {
    int result = -1;
    // Your solution here!
    return result;
}

int open64(const char *pathname, int flags, ...) {
    int result = -1;
    // Your solution here!
    return result;
}

int openat(int dirfd, const char *pathname, int flags, ...) {
    int result = -1;
    // Your solution here!
    return result;
}

int unlink(const char *pathname) {
    int result = -1;
    // Your solution here!
    return result;
}

int unlinkat(int dirfd, const char *pathname, int flags) {
    int result = -1;
    // Your solution here!
    return result;
}
