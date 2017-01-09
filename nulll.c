#include <linux/init.h>		/* __init and __exit macroses */
#include <linux/kernel.h>	/* KERN_INFO macros */
#include <linux/module.h>	/* required for all kernel modules */
#include <linux/moduleparam.h>	/* module_param() and MODULE_PARM_DESC() */

#include <linux/fs.h>		/* struct file_operations, struct file */
#include <linux/miscdevice.h>	/* struct miscdevice and misc_[de]register() */
#include <linux/mutex.h>	/* mutexes */
#include <linux/string.h>	/* memchr() function */
#include <linux/slab.h>		/* kzalloc() function */
#include <linux/sched.h>	/* wait queues */
#include <linux/uaccess.h>	/* copy_{to,from}_user() */

#include <linux/ioctl.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aliaksei Kasiyanik <aleksey.kasianik@gmail.com>");
MODULE_DESCRIPTION("/dev/nulll module");

static unsigned long capacity = 0;
module_param(capacity, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(capacity, "Capacity of device");

static unsigned long filled_bytes = 0;

static struct mutex lock;

static int nulll_open(struct inode *inode, struct file *file) {
	return 0;
}

static int nulll_close(struct inode *inode, struct file *file) {
	return 0;
}

static ssize_t nulll_write(struct file *file, const char __user * in, size_t size, loff_t * off) {
	// spin lock
	if (mutex_lock_interruptible(&lock)) {
        return -ERESTARTSYS;
	}
	if (filled_bytes == capacity && capacity != 0) {
		mutex_unlock(&lock);
		return -ENOSPC;
	}
	size_t write_size = size;
	if (filled_bytes + size > capacity && capacity != 0) {
		write_size = capacity - filled_bytes;
	} 
	filled_bytes += write_size;
	mutex_unlock(&lock);
	return write_size;
}

static long nulll_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) {
	switch (ioctl_num) {
		case BLKGETSIZE64: {
			if (copy_to_user((void __user *) ioctl_param, (const void *) &filled_bytes, sizeof(unsigned long))) {
				return -EFAULT;
			}
			break;
		}
		default: {
			return -ENOTTY;
		}
		// 'switch' instead of 'if' for further extending
	}
	return 0;
}

static struct file_operations nulll_fops = {
	.owner = THIS_MODULE,
	.open = nulll_open,
	.write = nulll_write,
	.release = nulll_close,
	.unlocked_ioctl = nulll_ioctl,
	.llseek = noop_llseek
};

static struct miscdevice nulll_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "nulll",
    .fops = &nulll_fops
};

static int __init nulll_init(void) {
	int ret_code = misc_register(&nulll_misc_device);
	if (ret_code) {
		printk(KERN_ERR
		"=== Unable to register /DEV/NULLL device\n"); 
		return ret_code;
	}

	mutex_init(&lock);
    printk(KERN_INFO
    "/DEV/NULLL device has been registered \n");
    return 0;
}

static void __exit nulll_exit(void) {
	mutex_destroy(&lock);
	misc_deregister(&nulll_misc_device);
    printk(KERN_INFO
    "/DEV/NULLL device has been unregistered \n");
}

module_init(nulll_init);
module_exit(nulll_exit);
