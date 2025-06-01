#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "chardev"
#define BUFFER_SIZE 512

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;
static char buffer[BUFFER_SIZE];
static int buffer_len = 0;

static int dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "chardev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "chardev: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t len, loff_t *off) {
    if (*off >= buffer_len)
        return 0;
    if (len > buffer_len - *off)
        len = buffer_len - *off;
    if (copy_to_user(user_buffer, buffer + *off, len))
        return -EFAULT;
    *off += len;
    printk(KERN_INFO "chardev: Read %zu bytes\n", len);
    return len;
}

static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *off) {
    if (len > BUFFER_SIZE - 1)
        len = BUFFER_SIZE - 1;
    if (copy_from_user(buffer, user_buffer, len))
        return -EFAULT;
    buffer_len = len;
    buffer[len] = '\0';
    printk(KERN_INFO "chardev: Wrote %zu bytes: %s\n", len, buffer);
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init chardev_init(void) {
    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ERR "chardev: Failed to allocate major number\n");
        return -1;
    }
    cdev_init(&my_cdev, &fops);
    if (cdev_add(&my_cdev, dev_num, 1) < 0) {
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "chardev: Failed to add cdev\n");
        return -1;
    }
    my_class = class_create(DEVICE_NAME);
    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "chardev: Failed to create class\n");
        return PTR_ERR(my_class);
    }
    if (!device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME)) {
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "chardev: Failed to create device\n");
        return -1;
    }
    printk(KERN_INFO "chardev: Initialized with Major %d\n", MAJOR(dev_num));
    return 0;
}

static void __exit chardev_exit(void) {
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "chardev: Exited\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirk");
MODULE_DESCRIPTION("Simple character device driver");