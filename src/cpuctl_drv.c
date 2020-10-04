#include <linux/module.h>	
#include <linux/kernel.h>	
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include "offset.h"
#include "ioctl.h"

MODULE_LICENSE("GPL");

static dev_t dev;
static struct cdev* cdev;
static struct class* cl;
static struct device* device;
static atomic_t available;

long cpuctl_ioctl(struct file* filp, unsigned int cmd, unsigned long arg);
int cpuctl_open(struct inode* inode, struct file* filp);
int cpuctl_release(struct inode* inode, struct file* filp);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = cpuctl_open,
	.release = cpuctl_release,
	.unlocked_ioctl = cpuctl_ioctl	
};

int init_module(void)
{
	int status = alloc_chrdev_region(&dev, 0, 1, "cpuctl"); 
	if (status < 0) {
		printk(KERN_ERR "cpuctl: Cannot allocate chrdev.\n");
		return -EFAULT;
	}
	printk(KERN_INFO "cpuctl: Registered major number: %d\n", MAJOR(dev));

	cdev = cdev_alloc();
	cdev->ops = &fops;
	
	cl = class_create(THIS_MODULE, "cpuctl_class");
	if (IS_ERR(cl)) {
		cdev_del(cdev);
		unregister_chrdev_region(dev, 1);
		printk(KERN_ERR "cpuctl: Cannot create device class\n");
		return -EFAULT;
	}
	printk(KERN_INFO "cpuctl: Created device class: cpuctl");

	device = device_create(cl, NULL, dev, NULL, "cpuctl");
	if (IS_ERR(device)) {
		class_destroy(cl);
		cdev_del(cdev);
		unregister_chrdev_region(dev, 1);
		printk(KERN_ERR "cpuctl: Cannot create device\n");
		return -EFAULT;
	}
	printk(KERN_INFO "cpuctl: Created device\n");

	atomic_set(&available, 1);
	status = cdev_add(cdev, dev, 1);
	if (status < 0) {
		device_destroy(cl, dev);
		class_unregister(cl);
		class_destroy(cl);
		cdev_del(cdev);
		unregister_chrdev_region(dev, 1);
		printk(KERN_ERR "cpuctl: Cannot add chrdev\n");
		return -EFAULT;
	}

	printk(KERN_INFO "cpuctl: loaded\n");

	return 0;
}

void cleanup_module(void)
{
	device_destroy(cl, dev);
	class_unregister(cl);
	class_destroy(cl);
	cdev_del(cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "cpuctl: unloaded\n");
}

int cpuctl_open(struct inode* inode, struct file* filp) {
	if (!atomic_dec_and_test(&available)) {
		atomic_inc(&available);
		printk(KERN_WARNING "cpuctl: Module already in use.\n");
		return -EBUSY;
	}
	return 0;
}

int cpuctl_release(struct inode* inode, struct file* filp) {
	atomic_inc(&available);
	return 0;
}

long cpuctl_ioctl(struct file* filp, unsigned int cmd, unsigned long arg) {
	struct offsets offsets;
	switch (cmd) {
		case CPUCTL_GET_OFFSETS:
			printk(KERN_INFO "cpuctl: Reading offsets\n");

			if (copy_from_user(&offsets, (void*)arg, sizeof(offsets))) {
				printk(KERN_ERR "cpuctl: copy_from_user error\n");
				return -EFAULT;
			}
			cpuctl_get_offsets(&offsets);
			if (copy_to_user((void*)arg, &offsets, sizeof(offsets))) {
				printk(KERN_INFO "cpuctl: Cannot write results to userspace.\n");
				return -EFAULT;
			}
			break;

		case CPUCTL_SET_OFFSETS:
			printk(KERN_INFO "cpuctl: Setting offsets\n");
			if (copy_from_user(&offsets, (void*)arg, sizeof(offsets))) {
				printk(KERN_ERR "cpuctl: copy_from_user error\n");
				return -EFAULT;
			}

			return cpuctl_set_offsets(&offsets);

		default:
			break;
	}

	return 0;
}

