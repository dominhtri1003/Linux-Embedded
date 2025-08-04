#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/reboot.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/string.h>

// Example of character device driver

#define MAGIC_NO        100
#define SET_SHUT_CMD    _IOW(MAGIC_NO, 0, char*)
#define SET_SHUT_TIME   _IOW(MAGIC_NO, 1, int)

MODULE_AUTHOR("Minh Tri Do");
MODULE_LICENSE("GPL");
MODULE_VERSION("1");

static dev_t dev;
static struct class *class_name;
static struct device *device_name;
static struct cdev my_cdev;

static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *filep, char __user *buf, size_t len, loff_t *offset);
static ssize_t dev_write(struct file *filep, const char __user *buf, size_t len, loff_t *offset);
//static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_close,
    .read = dev_read,
    .write = dev_write,
    .unlocked_ioctl = dev_ioctl,
};



static int dev_open(struct inode *inodep, struct file *filep)
{
    pr_info("Open is called\n");
    return 0;
}

static int dev_close(struct inode *inodep, struct file *filep)
{
    pr_info("Close is called\n");
    return 0;
}

static ssize_t dev_read(struct file*filep, char __user *buf, size_t len, loff_t *offset)
{
    printk("Read\n");
    return len;
}

char data[100];
static ssize_t dev_write(struct file*filep, const char __user *buf, size_t len, loff_t *offset)
{
    memset(data, 0, 100);
    if(copy_from_user(data, buf, len) != 0)
    {
	    printk(KERN_ERR "Failed to copy data from user");
    	    return -EFAULT;
    }
    printk("Write = %s\n", data);
    return len;
}

static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
        case SEND_DATA_CMD:
            memset(config_data, 0, IOCTL_DATA_LEN);
            copy_from_user(config_data, (char*)arg, IOCTL_DATA_LEN);
            printk(KERN_INFO "TriDo %s, %d, ioctl message: %s", __func__, __LINE__, config_data);
            break;
        default:
            return -ENOTTY;
    }
    return 0;
}

static int __init exam_init(void)
{
    int ret;
    // Xin cap phat boi so major, minor bat dau tu` 0 va` 1 so minor
    ret = alloc_chrdev_region(&dev, 0, 1, "example");
    if(ret)
    {
        printk("Can not registor major no\n");
        return ret;
    }
    pr_info("Register successfully major now is: %d\n", MAJOR(dev));
    // Allocate a cdev structure (cdev: Character device) - Khoi tao vung nho chua character device, tra ve con tro my_cdev
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE; 
    my_cdev.dev = dev;
    // Xin kernel cho dang ky device nay` vao` he thong
    ret = cdev_add(&my_cdev, dev, 1);

    if(ret < 0)
    {
        pr_info("cdev_add error\n");
        return ret;
    }

    class_name = class_create("hello");
    if(IS_ERR(class_name))
    {
        pr_info("Create class failed\n");
        return PTR_ERR(class_name);
    }
    pr_info("Create successfully class\n");

    device_name = device_create(class_name, NULL, dev, NULL, "ex_hello");
    if(IS_ERR(device_name))
    {
        pr_info("Create device failed\n");
        return PTR_ERR(device_name);
    }
    pr_info("Create device success\n");

    return 0;     
}

static void __exit exam_exit(void)
{
    pr_info("Goodbye\n");
    cdev_del(&my_cdev);
    device_destroy(class_name, dev);
    class_destroy(class_name);
    // Unallocate device number
    unregister_chrdev_region(dev, 1);
}

module_init(exam_init);
module_exit(exam_exit);

