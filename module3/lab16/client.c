#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


void error(const char *msg);
void send_file(int sockfd, const char *filepath);

int main(int argc, char *argv[]) {
    int sockfd, port, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[1024];

    printf("TCP Клиент\n");
    if (argc < 3) {
        fprintf(stderr, "Использование: %s <хост> <порт>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);
    // Создание сокета для TCP-соединения
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ошибка при открытии сокета");

    // Получение информации о хосте по его имени
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ошибка: хост не найден\n");
        exit(1);
    }

    // Инициализация структуры адреса сервера
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // Копирование адреса сервера из структуры hostent
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    // Установка порта сервера (конвертация в сетевой порядок байтов)
    serv_addr.sin_port = htons(port);

    // Установка соединения с сервером
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ошибка подключения");

    while (1) {
        printf("Введите команду ('+', '-', '*', '/', 'file' или 'exit'): ");
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer) - 1, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "exit") == 0) {
            printf("Завершение работы...\n");
            // Отправка команды exit серверу
            send(sockfd, buffer, strlen(buffer), 0);
            // Закрытие сокета при завершении работы
            close(sockfd);
            return 0;
        }

        if (strcmp(buffer, "file") == 0) {
            // Отправка команды file серверу
            send(sockfd, buffer, strlen(buffer), 0);
            printf("Введите путь к файлу: ");
            bzero(buffer, sizeof(buffer));
            fgets(buffer, sizeof(buffer) - 1, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            send_file(sockfd, buffer);
            continue;
        }

        if (strlen(buffer) != 1 || !strchr("+-*/", buffer[0])) {
            printf("Неверная команда. Используйте +, -, *, /, file или exit\n");
            continue;
        }

        // Отправка операции серверу
        send(sockfd, buffer, strlen(buffer), 0);

        printf("Введите первое число: ");
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer) - 1, stdin);
        // Отправка первого числа серверу
        send(sockfd, buffer, strlen(buffer), 0);

        printf("Введите второе число: ");
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer) - 1, stdin);
        // Отправка второго числа серверу
        send(sockfd, buffer, strlen(buffer), 0);

        bzero(buffer, sizeof(buffer));
        // Получение результата от сервера
        n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("ошибка получения данных\n");
            // Закрытие сокета при ошибке
            close(sockfd);
            return 1;
        }
        buffer[n] = 0;
        printf("Результат: %s\n", buffer);
    }

    // Закрытие сокета
    close(sockfd);
    return 0;
}

// Функция обработки ошибок при работе с сокетами
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Функция для отправки файла серверу
void send_file(int sockfd, const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        printf("Ошибка открытия файла\n");
        return;
    }

    // Получение имени файла из пути
    char *filename = strrchr(filepath, '/');
    filename = filename ? filename + 1 : (char *)filepath;

    // Отправка имени файла
    send(sockfd, filename, strlen(filename), 0);
    sleep(1); // Небольшая задержка для синхронизации

    // Чтение и отправка содержимого файла
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(sockfd, buffer, bytes_read, 0);
    }

    fclose(file);
    printf("Файл %s отправлен\n", filename);
}
