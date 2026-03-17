/*
 * Exercise 3 Solution — procfsutil.h (no-libc maps parser)
 */
#ifndef PROCFSUTIL_H
#define PROCFSUTIL_H

#include "minstr.h"
#include "syscall.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>

#define STACK_STR_PROC_SELF_MAPS(var)                                         \
  char var[16];                                                               \
  var[0] = '/';  var[1] = 'p';  var[2] = 'r';  var[3] = 'o';                \
  var[4] = 'c';  var[5] = '/';  var[6] = 's';  var[7] = 'e';                \
  var[8] = 'l';  var[9] = 'f';  var[10] = '/'; var[11] = 'm';               \
  var[12] = 'a'; var[13] = 'p'; var[14] = 's'; var[15] = '\0';

static int search_lib_procmaps(char *lib_needle, char *prot,
                               uintptr_t *base_out)
{
    char buf[4096];

    STACK_STR_PROC_SELF_MAPS(proc_self_maps);
    int fd = sys_openat(AT_FDCWD, proc_self_maps, O_RDONLY, 0);
    if (fd < 0)
        return -1;

    int bytes_read = sys_read(fd, buf, sizeof(buf) - 1);
    sys_close(fd);
    if (bytes_read <= 0)
        return -1;

    buf[bytes_read] = '\0';

    int line_start = 0;
    for (int i = 0; i < bytes_read; i++) {
        if (buf[i] == '\n' || i == bytes_read - 1) {
            char tmp = buf[i];
            buf[i] = '\0';

            char *line = &buf[line_start];

            /* Find end of address range (first space) */
            char *perms_start = line;
            while (*perms_start && *perms_start != ' ')
                perms_start++;

            if (*perms_start) {
                *perms_start = '\0';
                perms_start++;

                /* Check permissions */
                if (memcmp(perms_start, prot, strlen(prot)) == 0) {
                    /* Skip to pathname (after 5th space from perms) */
                    char *path = perms_start;
                    int spaces = 0;
                    while (*path && spaces < 5) {
                        if (*path == ' ')
                            spaces++;
                        path++;
                    }

                    if (strstr(path, lib_needle)) {
                        *base_out = hex_str_to_u64(line);
                        buf[i] = tmp;
                        return 0;
                    }
                }
            }

            buf[i] = tmp;
            line_start = i + 1;
        }
    }

    return -1;
}

#endif /* PROCFSUTIL_H */
