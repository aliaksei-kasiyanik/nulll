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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aliaksei Kasiyanik <aleksey.kasianik@gmail.com>");
MODULE_DESCRIPTION("/dev/nulll module");

static unsigned long capacity = 0;
module_param(capacity, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(capacity, "Capacity of device");

static int __init reverse_init(void) {
    printk(KERN_INFO
    "reverse device has been registeredn");
    return 0;
}

static void __exit reverse_exit(void) {
    printk(KERN_INFO
    "reverse device has been unregisteredn");
}

module_init(reverse_init);
module_exit(reverse_exit);