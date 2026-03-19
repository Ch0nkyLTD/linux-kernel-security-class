/*
 * test_harness.c — RWX memory loader
 *
 * Simulates what a kernel injection module does: mmap an RWX page,
 * read the bootstrap+SO payload into it, and jump to offset 0.
 *
 * Usage: ./test_harness <path_to_mempayload>
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path_to_mempayload>\n", argv[0]);
        return 1;
    }

    /* 1. Open the payload file */
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    struct stat st;
    if (fstat(fd, &st) < 0) { perror("fstat"); close(fd); return 1; }
    size_t sz = st.st_size;

    /* 2. Allocate RWX memory */
    void *mem = mmap(NULL, sz, PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    /* 3. Read payload into RWX page */
    ssize_t rd = read(fd, mem, sz);
    if (rd < 0 || (size_t)rd < sz) {
        perror("read");
        munmap(mem, sz);
        close(fd);
        return 1;
    }
    close(fd);

    /* 4. Jump to offset 0 — the bootstrap stub.
     *    Layout: [bootstrap 4KB] [rp.so N bytes]
     *    The bootstrap loads sz into x0 and branches into rp.so. */
    printf("[harness] Loaded %zu bytes into RWX @ %p\n", sz, mem);
    printf("[harness] Jumping to bootstrap at offset 0\n");

    void (*func)(void) = (void (*)(void))mem;
    func();

    printf("[harness] Returned. Cleaning up.\n");
    munmap(mem, sz);
    return 0;
}
