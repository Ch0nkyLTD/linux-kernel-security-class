/*
 * vuln_rw.c - Vulnerable "debug interface" character device
 *
 * Challenges 8 & 9: Arbitrary kernel read/write
 *
 * Simulates a debug/diagnostic interface that was accidentally left in a
 * production kernel driver. Provides arbitrary kernel memory read and write
 * via two ioctls with NO address validation.
 *
 * The vulnerability: any user can read/write any kernel address. An attacker
 * can walk kernel data structures, modify credentials, or overwrite global
 * kernel variables (like modprobe_path).
 *
 * Real-world parallel: CVE-2013-2094, CVE-2016-6187, and many Android
 * kernel driver bugs exposed similar unvalidated read/write primitives.
 *
 * FOR EDUCATIONAL USE ONLY — this driver is deliberately vulnerable.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "vuln_rw.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Course Instructor");
MODULE_DESCRIPTION("Vulnerable debug interface — arbitrary kernel read/write");
MODULE_VERSION("1.0");

#define DEVICE_NAME  "vuln_rw"
#define CLASS_NAME   "vuln_rw_class"

static dev_t          dev_num;
static struct cdev    rw_cdev;
static struct class   *dev_class;
static struct device  *dev_device;

static int vuln_rw_open(struct inode *inode, struct file *file)
{
	pr_info("vuln_rw: opened by PID %d (%s)\n",
		current->pid, current->comm);
	return 0;
}

static int vuln_rw_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long vuln_rw_ioctl(struct file *file, unsigned int cmd,
			   unsigned long arg)
{
	struct vuln_rw_request req;

	if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
		return -EFAULT;

	if (req.len == 0 || req.len > PAGE_SIZE) {
		pr_err("vuln_rw: invalid length %zu (max %lu)\n",
		       req.len, PAGE_SIZE);
		return -EINVAL;
	}

	/*
	 * BUG: No validation of req.kaddr. A proper driver would check
	 * that the address is within a safe range (e.g., the driver's own
	 * DMA buffers). This driver trusts user input completely.
	 */

	switch (cmd) {
	case VULN_KREAD: {
		void *kbuf;

		pr_info("vuln_rw: KREAD %zu bytes from %lx\n",
			req.len, req.kaddr);

		kbuf = kmalloc(req.len, GFP_KERNEL);
		if (!kbuf)
			return -ENOMEM;

		/* Read kernel memory safely — nofault handles unmapped pages */
		if (copy_from_kernel_nofault(kbuf, (void *)req.kaddr,
					     req.len)) {
			kfree(kbuf);
			return -EFAULT;
		}
		if (copy_to_user(req.ubuf, kbuf, req.len)) {
			kfree(kbuf);
			return -EFAULT;
		}
		kfree(kbuf);
		return 0;
	}

	case VULN_KWRITE:
		pr_info("vuln_rw: KWRITE %zu bytes to %lx\n",
			req.len, req.kaddr);

		/* Copy userspace data to kernel memory — no access check */
		if (copy_from_user((void *)req.kaddr, req.ubuf, req.len))
			return -EFAULT;
		return 0;

	default:
		return -ENOTTY;
	}
}

static const struct file_operations vuln_rw_fops = {
	.owner          = THIS_MODULE,
	.open           = vuln_rw_open,
	.release        = vuln_rw_release,
	.unlocked_ioctl = vuln_rw_ioctl,
};

static int __init vuln_rw_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		pr_err("vuln_rw: alloc_chrdev_region failed: %d\n", ret);
		return ret;
	}

	cdev_init(&rw_cdev, &vuln_rw_fops);
	rw_cdev.owner = THIS_MODULE;

	ret = cdev_add(&rw_cdev, dev_num, 1);
	if (ret < 0) {
		pr_err("vuln_rw: cdev_add failed: %d\n", ret);
		goto fail_cdev;
	}

	dev_class = class_create(CLASS_NAME);
	if (IS_ERR(dev_class)) {
		ret = PTR_ERR(dev_class);
		pr_err("vuln_rw: class_create failed: %d\n", ret);
		goto fail_class;
	}

	/* 0666 permissions — any user can open the device */
	dev_device = device_create(dev_class, NULL, dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(dev_device)) {
		ret = PTR_ERR(dev_device);
		pr_err("vuln_rw: device_create failed: %d\n", ret);
		goto fail_device;
	}

	pr_info("vuln_rw: created /dev/%s (major=%d, minor=%d)\n",
		DEVICE_NAME, MAJOR(dev_num), MINOR(dev_num));
	return 0;

fail_device:
	class_destroy(dev_class);
fail_class:
	cdev_del(&rw_cdev);
fail_cdev:
	unregister_chrdev_region(dev_num, 1);
	return ret;
}

static void __exit vuln_rw_exit(void)
{
	device_destroy(dev_class, dev_num);
	class_destroy(dev_class);
	cdev_del(&rw_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("vuln_rw: device /dev/%s removed\n", DEVICE_NAME);
}

module_init(vuln_rw_init);
module_exit(vuln_rw_exit);
