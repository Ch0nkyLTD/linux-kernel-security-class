# inclass/payload_staging — staging server + client demo
# All binaries are AArch64; run on host with qemu-aarch64 -L /usr/aarch64-linux-gnu/

CC     := aarch64-linux-gnu-gcc
CFLAGS := -Wall -Wextra -O2
QEMU   := qemu-aarch64 -L /usr/aarch64-linux-gnu/

all: server client payload

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

payload: payload.c
	$(CC) $(CFLAGS) -o payload payload.c

demo: all
	@rm -f pwned.txt
	@$(QEMU) ./server 4444 payload &
	@sleep 0.2
	@$(QEMU) ./client 127.0.0.1 4444
	@echo "--- pwned.txt ---"
	@cat pwned.txt

clean:
	rm -f server client payload pwned.txt

.PHONY: all demo clean
