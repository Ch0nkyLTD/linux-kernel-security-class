#!/bin/bash
# setup_capstone.sh — Capstone environment setup for the QEMU guest
#
# Run this INSIDE the AArch64 VM after copying files via shared folder:
#   sudo bash /mnt/shared/capstone/setup_capstone.sh
#
# What it does:
#   1. Creates analyst and director users
#   2. Populates /home/director/classified/ with secret files
#   3. Installs the MERIDIAN service
#   4. Loads vulnerable drivers and sets permissions
#   5. Sets kptr_restrict=0 for /proc/kallsyms access

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "=========================================="
echo " Capstone Environment Setup"
echo "=========================================="

# ── 1. Create users ──────────────────────────────────────────────────────────

echo "[*] Creating users..."

if ! id analyst &>/dev/null; then
    useradd -m -u 1001 -s /bin/bash analyst
    echo "analyst:analyst" | chpasswd
    echo "    Created user: analyst (uid 1001)"
else
    echo "    User analyst already exists"
fi

if ! id director &>/dev/null; then
    useradd -m -u 1002 -s /bin/bash director
    echo "director:classified" | chpasswd
    echo "    Created user: director (uid 1002)"
else
    echo "    User director already exists"
fi

# ── 1b. Install MOTD ────────────────────────────────────────────────────────

echo "[*] Installing MOTD..."

cat > /etc/motd << 'MOTD'
================================================================================
  WARNING: This is a MERIDIAN DEFENSE GROUP restricted system.
  Unauthorized access is prohibited. All activity is monitored and recorded.
  Disconnect immediately if you are not authorized personnel.
================================================================================
MOTD
echo "    MOTD installed"

# ── 2. Populate classified files ─────────────────────────────────────────────

echo "[*] Populating classified files..."

CLASSIFIED="/home/director/classified"
mkdir -p "$CLASSIFIED"

cat > "$CLASSIFIED/agents.txt" << 'AGENTS'
MERIDIAN DEFENSE GROUP — TOP SECRET // SCI // NOFORN

FIELD OPERATIVE REGISTRY — CURRENT DEPLOYMENTS

Operative         Cover Identity            Region               Status
─────────────────────────────────────────────────────────────────────────
CARDINAL          Commercial trader         Eastern Med          ACTIVE
RAVEN             NGO aid worker            Central Asia         ACTIVE
PHOENIX           Academic researcher       Southeast Asia       ACTIVE
VIPER             Maritime consultant       Horn of Africa       DORMANT
SPARROW           Tech company employee     Northern Europe      ACTIVE
FALCON            Diplomatic staff          Middle East          ACTIVE
CONDOR            Journalist (freelance)    South America        EXFIL

HANDLER NOTES:
- CARDINAL due for re-validation June 2024
- VIPER requested reactivation — pending security review
- CONDOR compromised 12 Apr 2024, emergency exfiltration complete
- SPARROW reporting cadence increased to weekly per DIRECTORIATE tasking

THIS DOCUMENT CONTAINS CODEWORD MATERIAL.
UNAUTHORIZED DISCLOSURE SUBJECT TO CRIMINAL PROSECUTION.

PIR-ALPHA: VIPER-HORNOFAFRICA
AGENTS
chmod 600 "$CLASSIFIED/agents.txt"

cat > "$CLASSIFIED/operation_blackbird.txt" << 'BLACKBIRD'
MERIDIAN DEFENSE GROUP — TOP SECRET // SCI

OPERATION BLACKBIRD — MISSION BRIEFING

Classification:  TOP SECRET // SCI // ORCON
Date:            01 March 2024
Authority:       Director, Special Programs

OBJECTIVE

Establish persistent signals collection capability targeting
encrypted communications infrastructure in the Caspian Basin
region.  Operation timeline: 6 months.

PHASE 1 — INFRASTRUCTURE (Months 1-2)
- Deploy covert collection nodes at three pre-surveyed locations
- Establish encrypted exfiltration channels via commercial ISPs
- Validate collection against known test signals

PHASE 2 — COLLECTION (Months 3-5)
- Begin passive collection on priority frequency bands
- Correlate intercepts with HUMINT reporting from CARDINAL and RAVEN
- Provide weekly summaries to Threat Analysis Center

PHASE 3 — EXPLOITATION (Month 6)
- Deliver final intelligence assessment to DIRECTORIATE
- Begin infrastructure teardown per exfiltration plan ECHO-7
- Debrief all participating personnel

BUDGET: $2.3M (FY2024 supplemental, line item 7741-SPEC)

RISKS:
- Host nation counterintelligence assessment: MODERATE
- Technical detection probability: LOW
- Source compromise risk: LOW (no HUMINT assets directly involved)

THIS DOCUMENT IS THE PROPERTY OF MERIDIAN DEFENSE GROUP.
REMOVAL FROM SECURE FACILITY PROHIBITED.

PIR-BRAVO: ECHO-7-CASPIAN
BLACKBIRD
chmod 600 "$CLASSIFIED/operation_blackbird.txt"

cat > "$CLASSIFIED/crypto_keys.txt" << 'KEYS'
MERIDIAN DEFENSE GROUP — TOP SECRET // SCI

CRYPTOGRAPHIC KEY MATERIAL — Q2 2024

Classification:   TOP SECRET // CRYPTO
Effective:        01 April 2024
Supersedes:       Q1 2024 key set (destroyed 31 March 2024)

FIELD COMMUNICATION KEYS

Designator     Algorithm    Key (hex)
──────────────────────────────────────────────────────────────
MERIDIAN-K1    AES-256      a4f7 2b91 c038 de56 7710 4f83 bb29 e0a1
                            3c58 9d72 f146 0ab5 e823 7d9f 4016 c5ea

MERIDIAN-K2    AES-256      0e73 b5a2 d981 4cf6 a237 50e8 1b94 fc63
                            d742 8e15 a0b9 63c7 5f21 de84 9a06 37fb

MERIDIAN-K3    ChaCha20     82a1 f574 0d39 b6e8 c253 7f90 14ad 6b5c
                            e847 2a63 d019 85fc b374 1e06 9258 af3d

EMERGENCY KEY  AES-256-GCM  ff01 9a82 4b73 c5d6 e8f0 2134 a7b9 5c6e
                            d013 8f24 b567 9a01 c3e5 4f78 1d92 06ab

NOTES:
- Keys are to be loaded into COMSEC equipment ONLY in secure facilities
- Compromised keys: report to COMSEC custodian immediately
- Destruction: crosscut shred and burn per MERIDIAN SOP 12-7

ALL KEY MATERIAL IS TOP SECRET // CRYPTO.
UNAUTHORIZED REPRODUCTION OR RETENTION IS A FEDERAL OFFENSE.

PIR-CHARLIE: K3-CHACHA20-82A1F574
KEYS
chmod 600 "$CLASSIFIED/crypto_keys.txt"

chown -R director:director "$CLASSIFIED"
chmod 700 "$CLASSIFIED"

echo "    Created 3 classified files in $CLASSIFIED"

# ── 3. Install MERIDIAN service ──────────────────────────────────────────────

echo "[*] Installing MERIDIAN service..."

if [ -f "$SCRIPT_DIR/meridian" ]; then
    cp "$SCRIPT_DIR/meridian" /usr/local/bin/meridian
    chmod 755 /usr/local/bin/meridian
    echo "    Installed /usr/local/bin/meridian"
else
    echo "    WARNING: meridian binary not found in $SCRIPT_DIR"
    echo "    Build with: cd hw/capstone && make service"
fi

# Install report files
mkdir -p /usr/local/share/meridian/reports
if [ -d "$SCRIPT_DIR/reports" ]; then
    cp "$SCRIPT_DIR/reports/"*.txt /usr/local/share/meridian/reports/ 2>/dev/null || true
    echo "    Installed report files"
fi

# Install systemd unit
if [ -f "$SCRIPT_DIR/meridian.service" ]; then
    cp "$SCRIPT_DIR/meridian.service" /etc/systemd/system/meridian.service
    systemctl daemon-reload
    systemctl enable meridian.service
    systemctl start meridian.service
    echo "    Started meridian.service"
else
    echo "    WARNING: meridian.service not found — start manually"
fi

# ── 3b. Install SNITCH IDS ───────────────────────────────────────────────────

echo "[*] Installing SNITCH host monitoring..."

# Install snitch kernel module
if [ -f "$SCRIPT_DIR/snitch.ko" ]; then
    insmod "$SCRIPT_DIR/snitch.ko" 2>/dev/null || true
    echo "    Loaded snitch.ko"
else
    echo "    WARNING: snitch.ko not found in $SCRIPT_DIR"
fi

# Install snitch_watcher binary
if [ -f "$SCRIPT_DIR/snitch_watcher" ]; then
    cp "$SCRIPT_DIR/snitch_watcher" /usr/local/bin/snitch_watcher
    chmod 755 /usr/local/bin/snitch_watcher
    echo "    Installed /usr/local/bin/snitch_watcher"
else
    echo "    WARNING: snitch_watcher binary not found in $SCRIPT_DIR"
fi

# Install snitch-watcher systemd unit
if [ -f "$SCRIPT_DIR/snitch-watcher.service" ]; then
    cp "$SCRIPT_DIR/snitch-watcher.service" /etc/systemd/system/snitch-watcher.service
    systemctl daemon-reload
    systemctl enable snitch-watcher.service
    systemctl start snitch-watcher.service
    echo "    Started snitch-watcher.service"
else
    echo "    WARNING: snitch-watcher.service not found — start manually"
fi

# ── 4. Load vulnerable drivers ──────────────────────────────────────────────

echo "[*] Loading vulnerable drivers..."

if [ -f "$SCRIPT_DIR/vuln_rwx.ko" ]; then
    insmod "$SCRIPT_DIR/vuln_rwx.ko" 2>/dev/null || true
    chmod 666 /dev/vuln_rwx 2>/dev/null || true
    echo "    Loaded vuln_rwx.ko"
else
    echo "    WARNING: vuln_rwx.ko not found"
fi

if [ -f "$SCRIPT_DIR/vuln_rw.ko" ]; then
    insmod "$SCRIPT_DIR/vuln_rw.ko" 2>/dev/null || true
    chmod 666 /dev/vuln_rw 2>/dev/null || true
    echo "    Loaded vuln_rw.ko"
else
    echo "    WARNING: vuln_rw.ko not found"
fi

# ── 5. Kernel settings ──────────────────────────────────────────────────────

echo "[*] Configuring kernel settings..."

echo 1 > /proc/sys/kernel/kptr_restrict
echo "    kptr_restrict = 1 (kallsyms hidden from non-root)"

echo 2 > /proc/sys/kernel/perf_event_paranoid
echo "    perf_event_paranoid = 2 (locked down)"

# ── Done ─────────────────────────────────────────────────────────────────────

echo ""
echo "=========================================="
echo " Setup complete!"
echo "=========================================="
echo ""
echo " MERIDIAN service: $(systemctl is-active meridian.service 2>/dev/null || echo 'unknown')"
echo " SNITCH module:   $(lsmod | grep -q '^snitch ' && echo 'loaded' || echo 'not loaded')"
echo " SNITCH watcher:  $(systemctl is-active snitch-watcher.service 2>/dev/null || echo 'unknown')"
echo " Test:  nc localhost 1337"
echo " Users: analyst (uid 1001), director (uid 1002)"
echo " Classified files: /home/director/classified/"
echo " Drivers: $(lsmod | grep -c vuln) loaded"
echo ""
