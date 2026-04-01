/*
 * vuln_rwx.c - Vulnerable "JIT engine" character device
 *
 * Challenge 7: RWX Kernel Shellcode
 *
 * Simulates a buggy JIT compiler that allocates executable kernel memory,
 * copies user-supplied code into it, and executes it in ring 0.
 *
 * The vulnerability: no validation of the supplied code. An attacker can
 * send arbitrary kernel shellcode (e.g., prepare_creds/commit_creds to
 * escalate privileges).
 *
 * Real-world parallel: JIT engines in eBPF, GPU drivers, and network
 * offload engines have had similar bugs where user-controlled bytecode
 * reaches executable kernel memory without proper verification.
 *
 * FOR EDUCATIONAL USE ONLY — this driver is deliberately vulnerable.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kprobes.h>
#include <linux/slab.h>
#include <asm/cacheflush.h>

#include "vuln_rwx.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Course Instructor");
MODULE_DESCRIPTION("Vulnerable JIT engine — executes user-supplied kernel shellcode");
MODULE_VERSION("1.0");

#define DEVICE_NAME  "vuln_rwx"
#define CLASS_NAME   "vuln_rwx_class"

static dev_t          dev_num;
static struct cdev    rwx_cdev;
static struct class   *dev_class;
static struct device  *dev_device;

/* module_alloc/module_memfree/set_memory_x are not exported — resolve via kprobe */
typedef void *(*module_alloc_t)(unsigned long size);
typedef void  (*module_memfree_t)(void *addr);
typedef int   (*set_memory_x_t)(unsigned long addr, int numpages);

static module_alloc_t   real_module_alloc;
static module_memfree_t real_module_memfree;
static set_memory_x_t   real_set_memory_x;

static unsigned long kprobe_lookup(const char *name)
{
	struct kprobe kp = { .symbol_name = name };
	unsigned long addr;

	if (register_kprobe(&kp) < 0)
		return 0;
	addr = (unsigned long)kp.addr;
	unregister_kprobe(&kp);
	return addr;
}

static int vuln_rwx_open(struct inode *inode, struct file *file)
{
	pr_info("vuln_rwx: opened by PID %d (%s)\n",
		current->pid, current->comm);
	return 0;
}

static int vuln_rwx_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long vuln_rwx_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	struct vuln_rwx_request req;
	void *buf;

	if (cmd != VULN_RWX_EXEC)
		return -ENOTTY;

	if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
		return -EFAULT;

	if (req.len == 0 || req.len > PAGE_SIZE) {
		pr_err("vuln_rwx: invalid length %zu (max %lu)\n",
		       req.len, PAGE_SIZE);
		return -EINVAL;
	}

	/* Step 1: Allocate executable kernel memory (like a JIT engine) */
	buf = real_module_alloc(req.len);
	if (!buf) {
		pr_err("vuln_rwx: module_alloc failed\n");
		return -ENOMEM;
	}

	/* Step 2: Copy user-supplied "bytecode" into kernel executable page */
	if (copy_from_user(buf, req.code, req.len)) {
		real_module_memfree(buf);
		return -EFAULT;
	}

	/* Step 3: Make the page executable (module_alloc returns NX pages) */
	real_set_memory_x((unsigned long)buf, 1);

	/* Step 4: Flush I-cache for AArch64 coherency */
	flush_icache_range((unsigned long)buf, (unsigned long)buf + req.len);

	pr_info("vuln_rwx: executing %zu bytes of 'JIT code' at %px\n",
		req.len, buf);

	/* Step 5: Execute the "compiled bytecode" — THE VULNERABILITY */
	((void (*)(void))buf)();

	/* Step 6: Free executable memory */
	real_module_memfree(buf);

	pr_info("vuln_rwx: JIT execution complete\n");
	return 0;
}

static const struct file_operations vuln_rwx_fops = {
	.owner          = THIS_MODULE,
	.open           = vuln_rwx_open,
	.release        = vuln_rwx_release,
	.unlocked_ioctl = vuln_rwx_ioctl,
};

static int __init vuln_rwx_init(void)
{
	int ret;

	/* Resolve unexported symbols via kprobe trick */
	real_module_alloc = (module_alloc_t)kprobe_lookup("module_alloc");
	if (!real_module_alloc) {
		pr_err("vuln_rwx: cannot resolve module_alloc\n");
		return -ENOENT;
	}

	real_module_memfree = (module_memfree_t)kprobe_lookup("module_memfree");
	if (!real_module_memfree) {
		pr_err("vuln_rwx: cannot resolve module_memfree\n");
		return -ENOENT;
	}

	real_set_memory_x = (set_memory_x_t)kprobe_lookup("set_memory_x");
	if (!real_set_memory_x) {
		pr_err("vuln_rwx: cannot resolve set_memory_x\n");
		return -ENOENT;
	}

	pr_info("vuln_rwx: module_alloc @ %px, module_memfree @ %px, set_memory_x @ %px\n",
		real_module_alloc, real_module_memfree, real_set_memory_x);

	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		pr_err("vuln_rwx: alloc_chrdev_region failed: %d\n", ret);
		return ret;
	}

	cdev_init(&rwx_cdev, &vuln_rwx_fops);
	rwx_cdev.owner = THIS_MODULE;

	ret = cdev_add(&rwx_cdev, dev_num, 1);
	if (ret < 0) {
		pr_err("vuln_rwx: cdev_add failed: %d\n", ret);
		goto fail_cdev;
	}

	dev_class = class_create(CLASS_NAME);
	if (IS_ERR(dev_class)) {
		ret = PTR_ERR(dev_class);
		pr_err("vuln_rwx: class_create failed: %d\n", ret);
		goto fail_class;
	}

	/* 0666 permissions — any user can open the device */
	dev_device = device_create(dev_class, NULL, dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(dev_device)) {
		ret = PTR_ERR(dev_device);
		pr_err("vuln_rwx: device_create failed: %d\n", ret);
		goto fail_device;
	}

	pr_info("vuln_rwx: created /dev/%s (major=%d, minor=%d)\n",
		DEVICE_NAME, MAJOR(dev_num), MINOR(dev_num));
	return 0;

fail_device:
	class_destroy(dev_class);
fail_class:
	cdev_del(&rwx_cdev);
fail_cdev:
	unregister_chrdev_region(dev_num, 1);
	return ret;
}

static void __exit vuln_rwx_exit(void)
{
	device_destroy(dev_class, dev_num);
	class_destroy(dev_class);
	cdev_del(&rwx_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("vuln_rwx: device /dev/%s removed\n", DEVICE_NAME);
}

module_init(vuln_rwx_init);
module_exit(vuln_rwx_exit);
