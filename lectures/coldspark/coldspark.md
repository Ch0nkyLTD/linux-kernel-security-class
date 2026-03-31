---
theme: black
transition: none
---

<!-- slide -->

## TOP SECRET // SCI // NOFORN

# COLDSPARK

### Capstone Target Briefing

---

<!-- slide -->

## Target: MERIDIAN DEFENSE GROUP

- Foreign private intelligence contractor
- Provides SIGINT, HUMINT, GEOINT to hostile state actors
- Maintains classified source networks and active operations
- Internal system accessible via secure terminal (TCP/1337)

---

<!-- slide -->

## Required Capabilities

| # | Capability | Method |
|---|-----------|--------|
| 1 | **Beachhead** | Exploit `submit` command for code execution |
| 2 | **Privilege Escalation** | Kernel driver → root |
| 3 | **Implant Installation** | Kernel module with covert C2 |
| 4 | **PSP Subversion** | Neutralize SNITCH monitoring |
| 5 | **Collection** | Exfiltrate classified materials |

---

<!-- slide -->

## Attack Surface

```
                    ┌─────────────────────┐
  nc :11337 ───────►│  MERIDIAN Terminal   │
                    │  submit → mmap RWX  │
                    │  clone → exec code  │
                    └────────┬────────────┘
                             │ analyst (uid 1001)
                    ┌────────▼────────────┐
                    │  /dev/vuln_rwx      │  JIT engine
                    │  /dev/vuln_rw       │  Debug interface
                    └────────┬────────────┘
                             │ root (uid 0)
                    ┌────────▼────────────┐
                    │  insmod rootkit.ko  │
                    │  C2: kill -62 <cmd> │
                    └────────┬────────────┘
                             │
                    ┌────────▼────────────┐
                    │  /home/director/    │
                    │  classified/        │
                    │  PIR-ALPHA/BRAVO/   │
                    │  CHARLIE            │
                    └─────────────────────┘
```

---

<!-- slide -->

## Priority Intelligence Requirements

| PIR | Requirement |
|-----|-------------|
| **ALPHA** | Identify operative deployed to Horn of Africa |
| **BRAVO** | Identify exfiltration plan for Caspian Basin op |
| **CHARLIE** | Recover ChaCha20 key designator + first 4 bytes |

Submit exact flag strings to Gradescope.

---

<!-- slide -->

## DEMO: Recon

```bash
nc localhost 11337
```

_Connect to MERIDIAN Secure Terminal._
_Browse reports, check clearance, find the `submit` command._

---

<!-- slide -->

## DEMO: Beachhead

```bash
python3 tools/send_shellcode.py beachhead_shellcode.bin
```

_Submit AArch64 shellcode → creates `/tmp/pwned`_
_Code execution achieved as analyst._

---

<!-- slide -->

## Escalation Surface

```bash
ls -la /dev/vuln_rwx /dev/vuln_rw
grep prepare_kernel_cred linux-6.6/System.map
```

- `/dev/vuln_rwx` — executes your shellcode in kernel mode
- `/dev/vuln_rw` — read/write any kernel address
- `System.map` — resolve symbols offline (no KASLR, `kallsyms` locked)

_How you get from analyst to root is your problem._

---

<!-- slide -->

## DEMO: The Implant

```bash
insmod rootkit.ko           # deploy
ls /tmp                     # rk_ files gone
lsmod | grep rootkit        # module gone
cat /tmp/capstone_protected # BLOCKED
kill -62 0                  # C2 beacon
kill -62 1                  # toggle hiding
sleep 9999 &
kill -62 $!                 # inject shellcode
```

_This is what you're building._

---

<!-- slide -->

## Defenses

- **SNITCH**: Kernel-resident monitoring agent
- Detects: module loading, hidden files, anomalous signals
- Baseline: `rmmod snitch` after escalation
- Challenge: evade without unloading

---

<!-- slide -->

## Testing Your Work

Four test scripts — run inside the VM:

```bash
sudo bash test/test_lkm.sh        # rootkit features
sudo bash test/test_challenges.sh  # exploit challenges
bash test/test_userland.sh         # LD_PRELOAD (optional)
sudo bash test/test_both.sh        # side-by-side comparison
```

- Tests check **your build output** — no solutions provided
- Stubs produce "not implemented" → tests skip gracefully
- Build with `make`, deploy with `make deploy`, test in VM
- Challenge tests handle partial progress (skip vs fail)

---

<!-- slide -->

## Timeline

- **Week 1**: Beachhead + escalation
- **Week 2**: Implant (rootkit module)
- **Week 3**: Integration, testing, PIR collection

Deliverables: working exploit chain + PIR flag strings

---

<!-- slide -->

## TOP SECRET // SCI // NOFORN

# Questions?

_Targeting package distributed separately._
