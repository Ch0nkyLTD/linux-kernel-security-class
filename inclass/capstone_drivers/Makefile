# ──────────────────────────────────────────────────────────────
# Makefile — Capstone Vulnerable Drivers
# ──────────────────────────────────────────────────────────────
#
# Builds both vulnerable drivers used in the capstone project:
#   vuln_rwx  — JIT engine (kernel shellcode execution)
#   vuln_rw   — Debug interface (arbitrary kernel read/write)
#
# Targets:
#   make          Build both drivers
#   make install  Build + deploy to QEMU shared folder
#   make clean    Remove build artifacts

.PHONY: all install clean

all:
	$(MAKE) -C vuln_rwx
	$(MAKE) -C vuln_rw

install:
	$(MAKE) -C vuln_rwx install
	$(MAKE) -C vuln_rw install

clean:
	$(MAKE) -C vuln_rwx clean
	$(MAKE) -C vuln_rw clean
