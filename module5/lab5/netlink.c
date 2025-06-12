#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define NETLINK_TEST 17

static struct sock *nl_sk = NULL;
static int my_netlink_pid = 0;

static void nl_send_msg(int pid, const char *msg)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int msg_size = strlen(msg) + 1;
    int res;

    skb = nlmsg_new(msg_size, GFP_KERNEL);
    if (!skb) {
        printk(KERN_ERR "Netlink: Ошибка создания skb\n");
        return;
    }

    nlh = nlmsg_put(skb, 0, 0, NLMSG_DONE, msg_size, 0);
    strncpy(nlmsg_data(nlh), msg, msg_size);

    NETLINK_CB(skb).dst_group = 0;
    res = nlmsg_unicast(nl_sk, skb, pid);
    if (res < 0) {
        printk(KERN_ERR "Netlink: Ошибка отправки сообщения\n");
    }
}

// Обработка входящих сообщений
static void nl_data_ready(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    char *msg;
    if (skb->len >= sizeof(struct nlmsghdr)) {
        nlh = (struct nlmsghdr *)skb->data;
        my_netlink_pid = nlh->nlmsg_pid;
        msg = (char *)NLMSG_DATA(nlh);
        printk(KERN_INFO "Netlink: Получено сообщение от pid %d: %s\n", my_netlink_pid, msg);

        if (strcmp(msg, "Привет от пользователя") == 0) {
            nl_send_msg(my_netlink_pid, "Привет из пространства ядра!");
        }
    }
}

static int __init chardev_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = nl_data_ready,
    };
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if (!nl_sk) {
        printk(KERN_ERR "Netlink: Ошибка создания сокета\n");
        return -1;
    }
    printk(KERN_INFO "Netlink: Модуль успешно загружен\n");
    return 0;
}

static void __exit chardev_exit(void)
{
    if (nl_sk) {
        netlink_kernel_release(nl_sk);
    }
    printk(KERN_INFO "Netlink: Модуль выгружен\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirk");
MODULE_DESCRIPTION("Netlink character device module for Ubuntu 22.04 kernel 6.8.0-60");