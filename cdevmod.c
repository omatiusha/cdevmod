#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/debugfs.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/kdev_t.h>

MODULE_DESCRIPTION("Base64 chardev encoder|decoder");
MODULE_AUTHOR("raibu");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");

static int major = 0;
module_param(major, int, 0);
MODULE_PARM_DESC(major, "Major number. Defaults to 0 (automatic allocation)");




static const char *devname = THIS_MODULE->name;

static dev_t my_dev = 0;    // Stores our device handle
static struct cdev my_cdev;    // scull-initialized
static struct class *cl;

/**
 * cdev_open() - callback for open() file operation
 * @inode: information to manipulate the file (unused)
 * @file: VFS file opened by a process
 *
 * 
 */
static int cdev_open(struct inode *inode, struct file *file)
{
    

    return 0;
}

/**
 * cdev_release() - file close() callback
 * @inode: information to manipulate the file (unused)
 * @file: VFS file opened by a process
 */
static int cdev_release(struct inode *inode, struct file *file)
{
    

    return 0;
}

/**
 * cdev_read() - called on file read() operation
 * @file: VFS file opened by a process
 * @buf:
 * @count:
 * @loff:
 */

static ssize_t cdev_read(struct file *file, char __user *buf, size_t count, loff_t *loff)
{
    const static char *output = "NOT PATCHED\n";
    unsigned long bytes_read = 0;
    unsigned long n = strlen(output);

    if(*loff > n){
        return 0;
    }
    if(*loff + count > n){
        count = n-*loff;
    }

    bytes_read = copy_to_user(buf, output+*loff, count);
    if(bytes_read) return -EFAULT;

    *loff+=count;

    return count;
}

static ssize_t cdev_write(struct file *file, const char __user *buf, size_t count, loff_t *loff)
{
    
    return 0;
}



// This structure is partially initialized here
// and the rest is initialized by the kernel after call
// to cdev_init()
static struct file_operations b64ed_fops = {
    .open = &cdev_open,
    .release = &cdev_release,
    .read = &cdev_read,
    .write = &cdev_write,
    // required to prevent module unloading while fops are in use
    .owner = THIS_MODULE,
};

static int __init cdevmod_init(void)
{
    int status = 0;

    pr_info("MODULE NAME [%s]", THIS_MODULE->name);

    if (0 == major) {
        // use dynamic allocation (automatic)
        status = alloc_chrdev_region(&my_dev, 0, 1, devname);

    } else {
        // stick with what user provided
        my_dev = MKDEV(major, 0);
        status = register_chrdev_region(my_dev, 1, devname);
    }
    if (status) {
        goto err_handler;
    }

    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL) {
        unregister_chrdev_region(my_dev, 1);
        status = -ENOTTY;
        goto err_handler;
    }

    if (device_create(cl, NULL, my_dev, NULL, "readcrap") == NULL) {
        class_destroy(cl);
        unregister_chrdev_region(my_dev, 1);
        status = -ENOTTY;
        goto err_handler;
    }

    cdev_init(&my_cdev, &b64ed_fops);
    if ((status = cdev_add(&my_cdev, my_dev, 1))) {
        device_destroy(cl, my_dev);
        class_destroy(cl);
        unregister_chrdev_region(my_dev, 1);
        goto err_handler;
    }
    pr_info("Registered device with %d:%d\n", MAJOR(my_dev), MINOR(my_dev));
    return 0;

err_handler:
    return status;
}

static void __exit cdevmod_exit(void)
{
    cdev_del(&my_cdev);
    device_destroy(cl, my_dev);
    class_destroy(cl);
    unregister_chrdev_region(my_dev, 1);

    
}

module_init(cdevmod_init);
module_exit(cdevmod_exit);


