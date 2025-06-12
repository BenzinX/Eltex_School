#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

#define NETLINK_TEST 17

int main()
{
    struct sockaddr_nl src_addr, dest_addr;
    struct nlmsghdr *nlh = NULL, *recv_nlh = NULL;
    struct iovec iov, recv_iov;
    int sock_fd;
    char *msg = "Привет от пользователя";
    char recv_buf[256] = {0};

    // Создание сокета
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if (sock_fd < 0) {
        perror("Ошибка создания сокета");
        return -1;
    }

    // Настройка адреса
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // Для ядра

    // Подготовка сообщения
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(strlen(msg)));
    nlh->nlmsg_len = NLMSG_SPACE(strlen(msg));
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    strcpy(NLMSG_DATA(nlh), msg);

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    struct msghdr msg_struct = { .msg_name = &dest_addr, .msg_namelen = sizeof(dest_addr), .msg_iov = &iov, .msg_iovlen = 1 };

    // Отправка сообщения
    printf("Отправка сообщения ядру\n");
    fflush(stdout); // Принудительный вывод
    sendmsg(sock_fd, &msg_struct, 0);

    // Ожидание ответа
    printf("Ожидание сообщения от ядра\n");
    fflush(stdout); // Принудительный вывод

    // Получение ответа
    recv_iov.iov_base = recv_buf;
    recv_iov.iov_len = sizeof(recv_buf);
    struct msghdr recv_msg = { .msg_name = &dest_addr, .msg_namelen = sizeof(dest_addr), .msg_iov = &recv_iov, .msg_iovlen = 1 };
    recvmsg(sock_fd, &recv_msg, 0);

    recv_nlh = (struct nlmsghdr *)recv_buf;
    printf("Получено сообщение: %s\n", (char *)NLMSG_DATA(recv_nlh));

    free(nlh);
    close(sock_fd);
    return 0;
}