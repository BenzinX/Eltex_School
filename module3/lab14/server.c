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

int main(int argc, char *argv[]) {
    int sockfd; // Дескриптор сокета
    char buffer[1000]; // Буфер для сообщений
    struct sockaddr_in servaddr, client1_addr, client2_addr; // Структуры для адресов сервера и клиентов
    int client1_connected = 0, client2_connected = 0; // Флаги подключения клиентов
    socklen_t clilen; // Размер структуры адреса клиента
    int port; // Переменная для номера порта

    // Проверяем, указан ли порт в аргументах командной строки
    if (argc != 2) {
        printf("Использование: %s <порт>\n", argv[0]);
        exit(1);
    }

    // Преобразуем строку порта в число
    port = atoi(argv[1]);
    if (port <= 0 || port > MAX_PORT) {
        printf("Неверный номер порта: %s\n", argv[1]);
        exit(1);
    }

    // Создаем UDP сокет
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Ошибка создания сокета");
        exit(1);
    }

    // Обнуляем структуру адреса сервера
    bzero(&servaddr, sizeof(servaddr)); // Для корректности инициализации
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); // Используем порт из аргумента
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Принимать соединения с любого адреса

    // Привязываем сокет к адресу
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Ошибка привязки сокета");
        close(sockfd);
        exit(1);
    }

    printf("Сервер запущен на порту %d, ожидание клиентов...\n", port);

    while (1) {
        // Основной цикл обработки сообщений
        clilen = sizeof(struct sockaddr_in);
        // Получаем сообщение от клиента
        int n = recvfrom(sockfd, buffer, 999, 0, (struct sockaddr*)&servaddr, &clilen);
        if (n < 0) {
            perror("Ошибка получения данных");
            continue;
        }
        buffer[n] = '\0'; // Завершаем строку

        // Регистрируем первого клиента
        if (!client1_connected) {
            memcpy(&client1_addr, &servaddr, clilen);
            client1_connected = 1;
            printf("Клиент 1 подключен\n");
            continue;
        // Регистрируем второго клиента, если он отличается от первого
        } else if (!client2_connected && memcmp(&client1_addr, &servaddr, clilen) != 0) {
            memcpy(&client2_addr, &servaddr, clilen);
            client2_connected = 1;
            printf("Клиент 2 подключен\n");
            continue;
        }

        // Если оба клиента подключены, перенаправляем сообщения
        if (client1_connected && client2_connected) {
            // Сообщение от клиента 1 отправляем клиенту 2
            if (memcmp(&client1_addr, &servaddr, clilen) == 0) {
                if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&client2_addr, sizeof(client2_addr)) < 0) {
                    perror("Ошибка отправки клиенту 2");
                } else {
                    printf("Сообщение от клиента 1 клиенту 2: %s\n", buffer);
                }
            // Сообщение от клиента 2 отправляем клиенту 1
            } else if (memcmp(&client2_addr, &servaddr, clilen) == 0) {
                if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&client1_addr, sizeof(client1_addr)) < 0) {
                    perror("Ошибка отправки клиенту 1");
                } else {
                    printf("Сообщение от клиента 2 клиенту 1: %s\n", buffer);
                }
            }
        }
    }

    // Закрываем сокет
    close(sockfd);
    return 0;
}
