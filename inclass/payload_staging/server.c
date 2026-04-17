/*
 * server.c — staging server
 *
 * Usage: ./server <port> <file>
 *
 * Wire format: [ 4B magic 0xFEEDC0DE ] [ 4B size ] [ size bytes payload ]
 * Serves the same file to every client that connects.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define STAGE_MAGIC 0xFEEDC0DE

struct stage_hdr {
    uint32_t magic;
    uint32_t size;
};

static int send_exact(int fd, const void *buf, size_t n) {
    const char *p = buf;
    while (n > 0) {
        ssize_t w = send(fd, p, n, 0);
        if (w <= 0) return -1;
        p += w; n -= w;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage: server <port> <file>\n");
        return 1;
    }

    int port = atoi(argv[1]);
    const char *path = argv[2];

    /* read payload file */
    FILE *f = fopen(path, "rb");
    if (!f) { perror("fopen"); return 1; }
    struct stat st;
    stat(path, &st);
    uint32_t fsz = (uint32_t)st.st_size;
    void *fbuf = malloc(fsz);
    if (fread(fbuf, 1, fsz, f) != fsz) { perror("fread"); return 1; }
    fclose(f);

    /* listening socket */
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    int one = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    struct sockaddr_in addr = {0};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    listen(lfd, 5);

    printf("serving %s (%u bytes) on port %d\n", path, fsz, port);

    for (;;) {
        int connfd = accept(lfd, NULL, NULL);
        if (connfd < 0) continue;

        struct stage_hdr hdr = { .magic = htonl(STAGE_MAGIC), .size = htonl(fsz) };
        send_exact(connfd, &hdr, sizeof(hdr));
        send_exact(connfd, fbuf, fsz);
        close(connfd);
        printf("sent payload to client\n");
    }
}
