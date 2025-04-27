#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_PORT 65535

// Функция для получения сообщений
void receive_messages(int sockfd) {
    char recvline[1000]; // Буфер для принятых сообщений
    int n;
    while (1) {
        // Получаем сообщение
        if ((n = recvfrom(sockfd, recvline, 999, 0, NULL, NULL)) < 0) {
            perror("Ошибка получения сообщения");
            continue;
        }
        recvline[n] = '\0'; // Завершаем строку
        printf("\nПолучено: %s\nВаше сообщение: ", recvline);
        fflush(stdout); // Очищаем буфер вывода
    }
}

int main(int argc, char **argv) {
    int sockfd; // Дескриптор сокета
    char sendline[1000]; // Буфер для отправляемых сообщений
    struct sockaddr_in servaddr, cliaddr; // Структуры для адресов сервера и клиента
    int port; // Переменная для номера порта

    // Проверяем наличие IP-адреса и порта в аргументах
    if (argc != 3) {
        printf("Использование: %s <IP-адрес сервера> <порт>\n", argv[0]);
        exit(1);
    }

    // Преобразуем строку порта в число
    port = atoi(argv[2]);
    if (port <= 0 || port > MAX_PORT) {
        printf("Неверный номер порта: %s\n", argv[2]);
        exit(1);
    }

    // Создаем UDP сокет
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Ошибка создания сокета");
        exit(1);
    }

    // Настраиваем адрес клиента
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(0); // Автоматический выбор порта
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Любой локальный адрес

    // Привязываем сокет к адресу
    if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
        perror("Ошибка привязки сокета");
        close(sockfd);
        exit(1);
    }

    // Настраиваем адрес сервера
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); // Используем порт из аргумента
    if (inet_aton(argv[1], &servaddr.sin_addr) == 0) {
        printf("Неверный IP-адрес: %s\n", argv[1]);
        close(sockfd);
        exit(1);
    }

    // Создаем дочерний процесс для получения сообщений
    pid_t pid = fork();
    if (pid < 0) {
        perror("Ошибка создания процесса");
        close(sockfd);
        exit(1);
    } else if (pid == 0) {
        receive_messages(sockfd); // Дочерний процесс получает сообщения
        exit(0);
    }

    // Основной процесс отправляет сообщения
    printf("Введите ваши сообщения (введите 'exit' для выхода):\n");
    while (1) {
        printf("Ваше сообщение: ");
        fgets(sendline, 1000, stdin);
        sendline[strcspn(sendline, "\n")] = '\0'; // Удаляем символ новой строки

        // Проверяем команду выхода
        if (strcmp(sendline, "exit") == 0) {
            break;
        }

        // Отправляем сообщение серверу
        if (sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            perror("Ошибка отправки сообщения");
            break;
        }
    }

    // Закрываем сокет
    close(sockfd);
    return 0;
}

