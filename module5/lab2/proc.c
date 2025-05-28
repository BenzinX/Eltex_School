#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define PROC_NAME "hello"
#define BUFFER_SIZE 1024

static char buffer[BUFFER_SIZE];
static size_t buffer_len = 0;

// Функция чтения из /proc/hello
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *ppos)
{
    if (*ppos >= buffer_len)
        return 0;

    if (copy_to_user(usr_buf, buffer, buffer_len))
        return -EFAULT;

    *ppos += buffer_len;
    return buffer_len;
}

// Функция записи в /proc/hello
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *ppos)
{
    if (count > BUFFER_SIZE)
        count = BUFFER_SIZE;

    if (copy_from_user(buffer, usr_buf, count))
        return -EFAULT;

    buffer[count] = '\0';
    buffer_len = count;

    printk(KERN_INFO "Module received: %s", buffer);
    *ppos += count;
    return count;
}

// Обновлённая структура для новых версий ядра
static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init hello_init(void)
{
    struct proc_dir_entry *entry;

    entry = proc_create(PROC_NAME, 0666, NULL, &proc_fops);

    if (!entry) {
        printk(KERN_ALERT "Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    snprintf(buffer, sizeof(buffer), "Hello from kernel!\n");
    buffer_len = strlen(buffer);

    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit hello_cleanup(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

module_init(hello_init);
module_exit(hello_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leontyev Kirill");
MODULE_DESCRIPTION("Proc example module for modern kernels");