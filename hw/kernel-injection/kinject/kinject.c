// SPDX-License-Identifier: GPL-2.0
/*
 * kinject.c — Kernel Process Injection Driver
 *
 * Five parts, progressive difficulty:
 *   echo "inject1 <pid>"     > /dev/kinject   Part 1: direct PC hijack
 *   echo "inject2 <pid>"     > /dev/kinject   Part 2: clone trampoline
 *   echo "scan <pid>"        > /dev/kinject   Part 3: walk page tables
 *   echo "inject_pte <pid>"  > /dev/kinject   Part 4: PTE stealth injection
 *   (Part 5: run inject_pte then scan — detect your own injection)
 *
 * Part 1 teaches direct PC hijack via vm_mmap — the target is consumed.
 * Part 2 adds a clone() trampoline so the target survives the injection.
 * Part 3 teaches AArch64 4-level page table walking and anomaly detection.
 * Part 4 teaches manual PTE construction — invisible to /proc/pid/maps.
 * Part 5 ties Parts 3 and 4 together: the scanner detects stealth PTEs.
 *
 * Usage:
 *   insmod kinject.ko
 *   sleep 9999 &
 *   echo "inject1 $!" > /dev/kinject
 *   cat /tmp/kinject1_<pid>    # proof file created by payload
 *   rmmod kinject
 */

#include <asm/pgtable-hwdef.h>
#include <asm/pgtable-prot.h>
#include <asm/ptrace.h>
#include <asm/tlbflush.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/completion.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/pgtable.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/sched/task.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/mman.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel process injection driver — 5-part progressive lab");

#define DEVICE_NAME     "kinject"
#define CLASS_NAME      "kinject_class"
#define MAX_CMD_LEN     64

/*
 * Virtual addresses for PTE-injected pages (Part 4).
 * Sits in the gap between heap and stack.
 */
#define INJECT_CODE_VA  0x200000000UL
#define INJECT_STACK_VA 0x200001000UL

/* =========================================================================
 * Section 1 — AArch64 instruction encoding helpers (provided)
 * =========================================================================
 *
 * These encode individual AArch64 instructions as 32-bit words.
 * Used by build_payload() and build_trampoline() to generate shellcode.
 *
 *   movz Xd, #imm16, LSL #shift  →  1_10_100101_hw_imm16_Rd
 *   movk Xd, #imm16, LSL #shift  →  1_11_100101_hw_imm16_Rd
 *   svc  #imm16                   →  11010100_000_imm16_00001
 *   br   Xn                       →  1101011_0000_11111_000000_Rn_00000
 *   cbz  Xn, offset               →  1_011010_0_imm19_Rt
 *   mov  Xd, Xn                   →  orr Xd, xzr, Xn
 */
static u32 encode_movz(int rd, u16 imm16, int shift)
{
	int hw = shift / 16;
	return 0xD2800000 | (hw << 21) | ((u32)imm16 << 5) | rd;
}

static u32 encode_movk(int rd, u16 imm16, int shift)
{
	int hw = shift / 16;
	return 0xF2800000 | (hw << 21) | ((u32)imm16 << 5) | rd;
}

static u32 encode_svc(u16 imm)
{
	return 0xD4000001 | ((u32)imm << 5);
}

static u32 encode_br(int rn)
{
	return 0xD61F0000 | (rn << 5);
}

static u32 encode_movn(int rd, u16 imm16, int shift)
{
	int hw = shift / 16;
	return 0x92800000 | (hw << 21) | ((u32)imm16 << 5) | rd;
}

static u32 encode_mov_reg(int rd, int rn)
{
	return 0xAA0003E0 | (rn << 16) | rd;
}

static u32 encode_cbz(int rt, int offset_insns)
{
	u32 imm19 = (u32)(offset_insns & 0x7FFFF);
	return 0xB4000000 | (imm19 << 5) | rt;
}

static int emit_load_imm64(u32 *buf, int idx, int rd, unsigned long addr)
{
	buf[idx++] = encode_movz(rd, (addr >> 0) & 0xFFFF, 0);
	buf[idx++] = encode_movk(rd, (addr >> 16) & 0xFFFF, 16);
	buf[idx++] = encode_movk(rd, (addr >> 32) & 0xFFFF, 32);
	buf[idx++] = encode_movk(rd, (addr >> 48) & 0xFFFF, 48);
	return idx;
}

/* =========================================================================
 * Section 2 — Shellcode builder (provided)
 * =========================================================================
 *
 * Generates AArch64 shellcode that:
 *   1. openat(AT_FDCWD, "/tmp/<prefix>_<pid>", O_WRONLY|O_CREAT|O_TRUNC, 0644)
 *   2. write(fd, "injected\n", 9)
 *   3. close(fd)
 *   4. ppoll(NULL, 0, NULL, NULL) — sleep forever
 *
 * The payload is position-dependent: string addresses are computed
 * relative to code_va (the VA where the payload will be mapped).
 *
 * Returns number of instructions written to buf.
 */
static int build_payload(u32 *buf, unsigned long code_va,
			 const char *prefix, pid_t pid)
{
	int i = 0;
	char path[48];
	const char *msg = "injected\n";
	int path_len, msg_len, pad;
	int data_offset_insns;
	unsigned long path_off, msg_off;

	/* openat(AT_FDCWD, path, O_WRONLY|O_CREAT|O_TRUNC, 0644) */
	buf[i++] = encode_movz(8, 56, 0);           /* x8 = __NR_openat */
	buf[i++] = encode_movz(0, 0xFF9C, 0);       /* x0 = AT_FDCWD = -100 */
	buf[i++] = encode_movk(0, 0xFFFF, 16);
	buf[i++] = encode_movk(0, 0xFFFF, 32);
	buf[i++] = encode_movk(0, 0xFFFF, 48);
	/* x1 = path address (placeholders, patched below) */
	buf[i++] = 0; /* [5] */
	buf[i++] = 0; /* [6] */
	buf[i++] = 0; /* [7] */
	buf[i++] = 0; /* [8] */
	buf[i++] = encode_movz(2, 0x0241, 0);       /* x2 = O_WRONLY|O_CREAT|O_TRUNC */
	buf[i++] = encode_movz(3, 0x01A4, 0);       /* x3 = 0644 */
	buf[i++] = encode_svc(0);

	/* write(fd, msg, len) */
	buf[i++] = encode_mov_reg(19, 0);            /* x19 = fd (save) */
	buf[i++] = encode_movz(8, 64, 0);            /* x8 = __NR_write */
	buf[i++] = encode_mov_reg(0, 19);             /* x0 = fd */
	buf[i++] = 0; /* [15] — msg addr placeholder */
	buf[i++] = 0; /* [16] */
	buf[i++] = 0; /* [17] */
	buf[i++] = 0; /* [18] */
	buf[i++] = encode_movz(2, strlen(msg), 0);   /* x2 = len */
	buf[i++] = encode_svc(0);

	/* close(fd) */
	buf[i++] = encode_movz(8, 57, 0);
	buf[i++] = encode_mov_reg(0, 19);
	buf[i++] = encode_svc(0);

	/* ppoll(NULL, 0, NULL, NULL) — sleep forever */
	buf[i++] = encode_movz(8, 73, 0);            /* x8 = __NR_ppoll */
	buf[i++] = encode_movz(0, 0, 0);
	buf[i++] = encode_movz(1, 0, 0);
	buf[i++] = encode_movz(2, 0, 0);
	buf[i++] = encode_movz(3, 0, 0);
	buf[i++] = encode_svc(0);
	buf[i++] = 0x14000000 | ((-6) & 0x03FFFFFF); /* b loop (-6 insns) */

	/* String data after code */
	data_offset_insns = i;
	snprintf(path, sizeof(path), "/tmp/%s_%d", prefix, pid);
	path_len = strlen(path) + 1;
	msg_len = strlen(msg) + 1;

	path_off = code_va + (data_offset_insns * 4);
	msg_off = path_off + ((path_len + 3) & ~3);

	memcpy(&buf[i], path, path_len);
	pad = (path_len + 3) / 4;
	i += pad;

	memcpy(&buf[i], msg, msg_len);
	pad = (msg_len + 3) / 4;
	i += pad;

	/* Patch path address placeholders (slots 5-8) */
	buf[5] = encode_movz(1, (path_off >> 0) & 0xFFFF, 0);
	buf[6] = encode_movk(1, (path_off >> 16) & 0xFFFF, 16);
	buf[7] = encode_movk(1, (path_off >> 32) & 0xFFFF, 32);
	buf[8] = encode_movk(1, (path_off >> 48) & 0xFFFF, 48);

	/* Patch msg address placeholders (slots 15-18) */
	buf[15] = encode_movz(1, (msg_off >> 0) & 0xFFFF, 0);
	buf[16] = encode_movk(1, (msg_off >> 16) & 0xFFFF, 16);
	buf[17] = encode_movk(1, (msg_off >> 32) & 0xFFFF, 32);
	buf[18] = encode_movk(1, (msg_off >> 48) & 0xFFFF, 48);

	return i;
}

/* =========================================================================
 * Section 3 — Clone trampoline builder (provided, Parts 2 and 4)
 * =========================================================================
 *
 * The clone() trampoline is written into an existing VM_EXEC page.
 * Before entry:
 *   x27 = payload VA (child jumps here)
 *   x28 = original PC (parent returns here)
 *
 * Logic:
 *   clone(CLONE_VM|CLONE_THREAD|CLONE_SIGHAND, stack_top, 0, 0, 0)
 *   if (x0 == 0) goto child    ← clone returns 0 in child
 *   movn x0, #3                ← parent: x0 = -4 = -EINTR
 *   br x28                     ← parent returns to original PC
 * child:
 *   br x27                     ← child jumps to payload
 *
 * Why set x0 = -EINTR?  The parent returns to the instruction after the
 * interrupted syscall (e.g., nanosleep).  The syscall wrapper expects x0
 * to be the raw syscall return.  The kernel sets x0 to an internal errno
 * like -ERESTART_RESTARTBLOCK (-516) which should never reach userspace.
 * Because we set syscallno = NO_SYSCALL, the signal handling path skips
 * the conversion to -EINTR.  The trampoline fixes this: it overwrites
 * x0 with -EINTR before the parent resumes, so the wrapper sees a clean
 * return and restarts the sleep normally.
 */
static int build_trampoline(u32 *buf, unsigned long stack_va)
{
	int i = 0;
	unsigned long stack_top = stack_va + PAGE_SIZE - 16;

	/* x0 = CLONE_VM(0x100)|CLONE_THREAD(0x10000)|CLONE_SIGHAND(0x800) = 0x10900 */
	buf[i++] = encode_movz(0, 0x0900, 0);
	buf[i++] = encode_movk(0, 0x0001, 16);

	i = emit_load_imm64(buf, i, 1, stack_top);   /* x1 = stack top */

	buf[i++] = encode_movz(2, 0, 0);              /* x2 = 0 (parent_tidptr) */
	buf[i++] = encode_movz(3, 0, 0);              /* x3 = 0 (tls) */
	buf[i++] = encode_movz(4, 0, 0);              /* x4 = 0 (child_tidptr) */
	buf[i++] = encode_movz(8, 220, 0);            /* x8 = __NR_clone */
	buf[i++] = encode_svc(0);

	buf[i++] = encode_cbz(0, 3);                  /* cbz x0, child (+3) */
	buf[i++] = encode_movn(0, 3, 0);              /* parent: x0 = ~3 = -4 = -EINTR */
	buf[i++] = encode_br(28);                     /* parent: br x28 */
	buf[i++] = encode_br(27);                     /* child:  br x27 */

	return i;
}

/* =========================================================================
 * Section 4 — VMA helpers (provided)
 * ========================================================================= */

/*
 * Check that [addr, addr+len) doesn't overlap any VMA.
 */
static bool vma_gap_available(struct mm_struct *mm, unsigned long addr,
			      unsigned long len)
{
	struct vm_area_struct *vma;
	VMA_ITERATOR(iter, mm, addr);

	for_each_vma(iter, vma) {
		if (vma->vm_start >= addr + len)
			break;
		return false;
	}
	return true;
}

/*
 * Find a VM_EXEC page for the clone trampoline (Parts 2 and 4).
 * Returns address offset 0x100 bytes into the VMA, or 0 on failure.
 */
static unsigned long find_exec_addr(struct task_struct *task, size_t need)
{
	struct vm_area_struct *vma;
	unsigned long addr = 0;

	if (!task->mm)
		return 0;

	mmap_read_lock(task->mm);
	VMA_ITERATOR(iter, task->mm, 0);
	for_each_vma(iter, vma) {
		if (!(vma->vm_flags & VM_EXEC))
			continue;
		if (vma->vm_start + 0x100 + need > vma->vm_end)
			continue;
		addr = vma->vm_start + 0x100;
		break;
	}
	mmap_read_unlock(task->mm);
	return addr;
}

/* =========================================================================
 * PART 1 — Direct PC Hijack (student implements)
 * =========================================================================
 *
 * Simplest injection: allocate an RWX page in the target via vm_mmap(),
 * write the shellcode payload, then hijack the target's PC directly.
 * The target process becomes the payload — original behavior is destroyed.
 *
 * Why a workqueue?  vm_mmap() operates on current->mm.  To create a
 * mapping in the TARGET's mm, we need current->mm == target->mm.
 * kthread_use_mm() does this, but requires current->mm == NULL (kthread).
 * Workqueue workers are kthreads — perfect launch context.
 */

struct vma_inject_ctx {
	struct work_struct work;
	struct completion done;
	struct task_struct *target;
	pid_t target_pid;
	const char *prefix;
	bool alloc_stack;
	unsigned long inject_addr;
	unsigned long stack_addr;
	int result;
};

static void vma_inject_worker(struct work_struct *work)
{
	struct vma_inject_ctx *ctx =
		container_of(work, struct vma_inject_ctx, work);
	struct mm_struct *mm = ctx->target->mm;
	u32 payload_buf[128];
	int payload_len;
	unsigned long inject_addr = 0;

	/*
	 * Build the payload shellcode.
	 * Position-dependent: addresses are computed for the inject_addr
	 * that vm_mmap returns.  We don't know inject_addr yet, so we'll
	 * build the payload AFTER the mmap (see below).
	 */

	mmgrab(mm);
	kthread_use_mm(mm);

	/*
	 * Allocate an RWX page in the target's address space, then
	 * write the shellcode payload into it.
	 *
	 * vm_mmap() creates a VMA-backed mapping.  Since we called
	 * kthread_use_mm(target->mm), current->mm is the target's mm,
	 * so the mapping appears in the target's /proc/pid/maps.
	 *
	 * After the mmap, build the payload (position-dependent — needs
	 * the actual inject_addr), then copy_to_user to write it.
	 */
    // TODO: Your implementation here

	/* Allocate stack page for clone trampoline (Part 2 only) */
	if (ctx->alloc_stack) {
		unsigned long stack_addr;

		stack_addr = vm_mmap(NULL, 0, PAGE_SIZE,
				     PROT_READ | PROT_WRITE,
				     MAP_ANONYMOUS | MAP_PRIVATE, 0);
		if (IS_ERR_VALUE(stack_addr)) {
			pr_err("kinject: stack vm_mmap failed: %ld\n",
			       (long)stack_addr);
			ctx->result = (int)stack_addr;
			kthread_unuse_mm(mm);
			mmdrop(mm);
			complete(&ctx->done);
			return;
		}
		ctx->stack_addr = stack_addr;
		pr_info("kinject: stack mapped at %lx\n", stack_addr);
	}

	kthread_unuse_mm(mm);
	mmdrop(mm);

	ctx->inject_addr = inject_addr;
	ctx->result = 0;
	complete(&ctx->done);
}

static int do_inject1(pid_t pid)
{
	struct task_struct *task;
	struct vma_inject_ctx ctx;
	struct pt_regs *regs;

	/* Find target (scaffolding) */
	rcu_read_lock();
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (task)
		get_task_struct(task);
	rcu_read_unlock();

	if (!task) {
		pr_err("kinject: inject1: PID %d not found\n", pid);
		return -ESRCH;
	}
	if (!task->mm) {
		pr_err("kinject: inject1: PID %d has no mm\n", pid);
		put_task_struct(task);
		return -EINVAL;
	}

	pr_info("kinject: direct inject target PID %d (%s)\n",
		pid, task->comm);

	/* Schedule vm_mmap work on a kthread (scaffolding) */
	INIT_WORK(&ctx.work, vma_inject_worker);
	init_completion(&ctx.done);
	ctx.target = task;
	ctx.target_pid = pid;
	ctx.prefix = "kinject1";
	ctx.alloc_stack = false;
	ctx.inject_addr = 0;
	ctx.stack_addr = 0;
	ctx.result = 0;

	schedule_work(&ctx.work);
	wait_for_completion(&ctx.done);

	if (ctx.result) {
		put_task_struct(task);
		return ctx.result;
	}

	/*
	 * Hijack the target's PC to jump directly to the injected payload.
	 *
	 * task_pt_regs(task) gives the saved register state.  When the
	 * target returns to userspace, it will resume at regs->pc.
	 *
	 * Setting syscallno to ~0UL (NO_SYSCALL) prevents the kernel from
	 * restarting the interrupted syscall (which would overwrite PC).
	 *
	 * Setting TIF_SIGPENDING forces signal_pending() to return true,
	 * making the sleeping syscall return -EINTR so the target wakes
	 * and returns to userspace — at our modified PC.
	 */
    // TODO: Your implementation here

	put_task_struct(task);
	pr_info("kinject: direct injection complete for PID %d\n", pid);
	return 0;
}

/* =========================================================================
 * PART 2 — Clone Trampoline (student implements)
 * =========================================================================
 *
 * Same vm_mmap approach as Part 1, but now uses a clone() trampoline
 * so the target process survives the injection.  The trampoline:
 *   - Calls clone() to spawn a child thread
 *   - Parent resumes at its original PC (undisturbed)
 *   - Child runs the payload
 *
 * Memory: vm_mmap allocates code and stack pages (reuses Part 1 worker).
 * Execution: clone trampoline is written into an existing VM_EXEC page
 * via access_process_vm with FOLL_WRITE|FOLL_FORCE (triggers COW).
 */
static int do_inject2(pid_t pid)
{
	struct task_struct *task;
	struct vma_inject_ctx ctx;
	struct pt_regs *regs;
	u32 trampoline_buf[32];
	int tramp_len;
	unsigned long exec_addr;
	int ret;

	/* Find target (scaffolding) */
	rcu_read_lock();
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (task)
		get_task_struct(task);
	rcu_read_unlock();

	if (!task) {
		pr_err("kinject: inject2: PID %d not found\n", pid);
		return -ESRCH;
	}
	if (!task->mm) {
		pr_err("kinject: inject2: PID %d has no mm\n", pid);
		put_task_struct(task);
		return -EINVAL;
	}

	pr_info("kinject: clone inject target PID %d (%s)\n",
		pid, task->comm);

	/* Schedule vm_mmap work — code page + stack page (scaffolding) */
	INIT_WORK(&ctx.work, vma_inject_worker);
	init_completion(&ctx.done);
	ctx.target = task;
	ctx.target_pid = pid;
	ctx.prefix = "kinject2";
	ctx.alloc_stack = true;
	ctx.inject_addr = 0;
	ctx.stack_addr = 0;
	ctx.result = 0;

	schedule_work(&ctx.work);
	wait_for_completion(&ctx.done);

	if (ctx.result) {
		put_task_struct(task);
		return ctx.result;
	}

	/* Build clone trampoline using the stack page (scaffolding) */
	tramp_len = build_trampoline(trampoline_buf, ctx.stack_addr);

	/*
	 * Write the clone trampoline into an existing VM_EXEC page.
	 *
	 * find_exec_addr() locates a suitable region in the target's
	 * executable pages.  access_process_vm with FOLL_WRITE|FOLL_FORCE
	 * triggers COW on the read-only executable page, giving us a
	 * writable copy where we place the trampoline.
	 */
    // TODO: Your implementation here

	/*
	 * Hijack the target's PC with clone registers.
	 *
	 * Unlike Part 1's direct hijack, we redirect to the trampoline:
	 *   x28 = original PC (parent uses br x28 to resume)
	 *   x27 = payload VA  (child uses br x27 to run payload)
	 *   PC  = trampoline address
	 *
	 * The trampoline calls clone().  The parent returns to its
	 * original PC undisturbed.  The child runs the payload.
	 */
    // TODO: Your implementation here

	put_task_struct(task);
	return 0;
}

/* =========================================================================
 * PART 3 — Page Table Walking / Detection (student implements)
 * =========================================================================
 *
 * Walk a target process's 4-level page tables and report:
 *   - Total valid user PTEs
 *   - Any PTEs with no backing VMA (anomalies)
 *
 * AArch64 4KB granule, 48-bit VA, 4-level walk:
 *   PGD [47:39] → PUD [38:30] → PMD [29:21] → PTE [20:12] → offset [11:0]
 *
 * P4D is folded into PGD on AArch64 48-bit (p4d_offset is identity).
 *
 * At each level, if the entry is empty (*_none), skip to the next
 * aligned address.  Handle huge pages (pud_sect, pmd_sect) by skipping.
 *
 * For each valid PTE, check if find_vma(mm, va) covers it.
 * If not (no VMA or VMA starts after va) it's anomalous.
 *
 * This is the detection side — the tool that catches Part 4's stealth
 * injection.  Part 5 tests that this scanner detects the PTE anomalies.
 */
static int scan_ptes(pid_t pid)
{
	struct task_struct *task;
	struct mm_struct *mm;
	unsigned long va, task_size;
	int pte_count = 0, anomaly_count = 0;
	pgd_t *pgdp;
	p4d_t *p4dp;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;

    // TODO: Your implementation here
	return 0;
}

/* =========================================================================
 * PART 4 — Manual PTE Construction (student implements, challenge)
 * =========================================================================
 *
 * Build page table entries for a user page without any VMA backing.
 * The mapping is invisible to /proc/pid/maps.
 *
 * AArch64 4KB granule, 48-bit VA, 4-level walk:
 *   PGD [47:39] → PUD [38:30] → PMD [29:21] → PTE [20:12]
 *
 * At each level, if the entry is empty, we allocate a new page table
 * page and populate the entry.  The final PTE maps the physical page
 * with user RWX permissions (PAGE_SHARED_EXEC).
 *
 * Why WRITE_ONCE instead of pud_populate/pmd_populate?
 *   The kernel's populate helpers reference init_mm/swapper_pg_dir
 *   — symbols not exported to modules.  Since we only operate on
 *   user mm, WRITE_ONCE + dsb/isb is equivalent.
 *
 * P4D is folded into PGD on AArch64 48-bit.
 */
static int map_user_page(struct mm_struct *mm, unsigned long va,
			 struct page *page)
{
	pgd_t *pgdp;
	p4d_t *p4dp;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;
	pte_t pte;

    // TODO: Your implementation here
	return 0;
}

/*
 * PTE-based stealth injection.
 *
 * Unlike Parts 1 and 2, this creates page table entries directly — no VMA.
 * The mapping is invisible to /proc/pid/maps, making this a stealth
 * injection technique.
 *
 * Uses a clone() trampoline to spawn a new thread:
 *   - Parent resumes at its original PC (undisturbed)
 *   - Child runs the payload on the injected page
 *
 * Dual-mapping trick: we write the payload via the kernel linear map
 * (page_address), then create a user PTE to the same physical page.
 * Same bytes, two virtual addresses.
 */
static int do_inject_pte(pid_t pid)
{
	struct task_struct *task;
	struct mm_struct *mm;
	struct page *code_page, *stack_page;
	struct pt_regs *regs;
	u32 payload_buf[128], trampoline_buf[32];
	int payload_len, tramp_len;
	unsigned long exec_addr;
	int ret;

	/* Find target (scaffolding) */
	rcu_read_lock();
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (task)
		get_task_struct(task);
	rcu_read_unlock();

	if (!task) {
		pr_err("kinject: inject_pte: PID %d not found\n", pid);
		return -ESRCH;
	}
	mm = task->mm;
	if (!mm) {
		pr_err("kinject: inject_pte: PID %d has no mm\n", pid);
		put_task_struct(task);
		return -EINVAL;
	}

	pr_info("kinject: PTE inject target PID %d (%s)\n", pid, task->comm);

	/* Build shellcode (scaffolding) */
	payload_len = build_payload(payload_buf, INJECT_CODE_VA,
				    "kinject_pte", pid);
	tramp_len = build_trampoline(trampoline_buf, INJECT_STACK_VA);

	/*
	 * Allocate physical pages, write the payload via the kernel
	 * linear map (dual-mapping trick), verify VMA gap, install PTEs.
	 *
	 * alloc_page gives raw physical pages — no VMA, no mapping yet.
	 * page_address() gives the kernel VA for memcpy.
	 * map_user_page() creates PTEs at INJECT_CODE_VA / INJECT_STACK_VA.
	 */
    // TODO: Your implementation here

	/*
	 * Write the clone trampoline into an existing VM_EXEC page,
	 * then hijack the target's PC with clone registers.
	 *
	 * Same pattern as Part 2:
	 *   find_exec_addr → access_process_vm(FOLL_WRITE|FOLL_FORCE)
	 *   x28 = original PC, x27 = payload VA, PC = trampoline
	 */
    // TODO: Your implementation here

	put_task_struct(task);
	return 0;
}

/* =========================================================================
 * Section 9 — Character device (provided)
 * ========================================================================= */

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *dev_class;
static struct device *dev_device;

static int kinject_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int kinject_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t kinject_read(struct file *file, char __user *buf,
			    size_t count, loff_t *ppos)
{
	const char *usage =
		"usage:\n"
		"  echo \"inject1 <pid>\"     > /dev/kinject\n"
		"  echo \"inject2 <pid>\"     > /dev/kinject\n"
		"  echo \"scan <pid>\"        > /dev/kinject\n"
		"  echo \"inject_pte <pid>\"  > /dev/kinject\n";
	size_t len = strlen(usage);

	if (*ppos > 0)
		return 0;
	if (count < len)
		len = count;
	if (copy_to_user(buf, usage, len))
		return -EFAULT;
	*ppos += len;
	return len;
}

static ssize_t kinject_write(struct file *file, const char __user *buf,
			     size_t count, loff_t *ppos)
{
	char cmd[MAX_CMD_LEN] = {0};
	pid_t pid;
	int ret;
	size_t len = min(count, sizeof(cmd) - 1);

	if (copy_from_user(cmd, buf, len))
		return -EFAULT;
	cmd[len] = '\0';

	if (sscanf(cmd, "inject_pte %d", &pid) == 1) {
		ret = do_inject_pte(pid);
		return ret ? ret : count;
	}

	if (sscanf(cmd, "inject1 %d", &pid) == 1) {
		ret = do_inject1(pid);
		return ret ? ret : count;
	}

	if (sscanf(cmd, "inject2 %d", &pid) == 1) {
		ret = do_inject2(pid);
		return ret ? ret : count;
	}

	if (sscanf(cmd, "scan %d", &pid) == 1) {
		ret = scan_ptes(pid);
		return ret ? ret : count;
	}

	pr_warn("kinject: unknown command: %s", cmd);
	return -EINVAL;
}

static const struct file_operations kinject_fops = {
	.owner   = THIS_MODULE,
	.open    = kinject_open,
	.release = kinject_release,
	.read    = kinject_read,
	.write   = kinject_write,
};

/* =========================================================================
 * Section 10 — Module init / exit (provided)
 * ========================================================================= */

static int __init kinject_init(void)
{
	int ret;

	pr_info("kinject: loading — kernel process injection driver\n");

	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		pr_err("kinject: alloc_chrdev_region failed: %d\n", ret);
		return ret;
	}

	cdev_init(&my_cdev, &kinject_fops);
	my_cdev.owner = THIS_MODULE;

	ret = cdev_add(&my_cdev, dev_num, 1);
	if (ret < 0) {
		pr_err("kinject: cdev_add failed: %d\n", ret);
		goto fail_cdev;
	}

	dev_class = class_create(CLASS_NAME);
	if (IS_ERR(dev_class)) {
		ret = PTR_ERR(dev_class);
		pr_err("kinject: class_create failed: %d\n", ret);
		goto fail_class;
	}

	dev_device = device_create(dev_class, NULL, dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(dev_device)) {
		ret = PTR_ERR(dev_device);
		pr_err("kinject: device_create failed: %d\n", ret);
		goto fail_device;
	}

	pr_info("kinject: /dev/%s created\n", DEVICE_NAME);
	pr_info("kinject: commands: inject1, inject2, scan, inject_pte\n");
	return 0;

fail_device:
	class_destroy(dev_class);
fail_class:
	cdev_del(&my_cdev);
fail_cdev:
	unregister_chrdev_region(dev_num, 1);
	return ret;
}

static void __exit kinject_exit(void)
{
	device_destroy(dev_class, dev_num);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev_num, 1);

	pr_info("kinject: unloaded (injected threads/pages remain)\n");
}

module_init(kinject_init);
module_exit(kinject_exit);
