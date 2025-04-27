#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>

// Объявления функций
void error(const char *msg);
void printusers();
double calculate(char op, double a, double b);

int nclients = 0;

// Структура для хранения состояния клиента
typedef struct {
    int sockfd;           // Файловый дескриптор сокета
    char client_ip[INET_ADDRSTRLEN]; // IP-адрес клиента
    char client_hostname[256]; // Имя хоста клиента
    char step;            // Текущий шаг: 0 - ожидание операции, 1 - первое число, 2 - второе число
    char op;              // Операция (+, -, *, /)
    double a, b;          // Числа для вычисления
    char buffer[1024];    // Буфер для данных
} Client;

int main(int argc, char *argv[]) {
    int sockfd, portno;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    fd_set master_fds, read_fds;
    int fd_max;
    Client clients[FD_SETSIZE]; // Массив для хранения клиентов
    int client_count = 0;       // Текущее количество клиентов

    printf("TCP Сервер с мультиплексированием (select)\n");
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <порт>\n", argv[0]);
        exit(1);
    }

    // Создание сокета
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ошибка при открытии сокета");

    // Инициализация структуры адреса сервера
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Привязка сокета
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ошибка при привязке сокета");

    // Ожидание входящих соединений
    listen(sockfd, 5);

    // Инициализация набора дескрипторов
    FD_ZERO(&master_fds);
    FD_SET(sockfd, &master_fds);
    fd_max = sockfd;

    // Инициализация массива клиентов
    for (int i = 0; i < FD_SETSIZE; i++) {
        clients[i].sockfd = -1;
    }

    while (1) {
        read_fds = master_fds;
        if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) < 0)
            error("ошибка при вызове select");

        // Проверка всех дескрипторов
        for (int i = 0; i <= fd_max; i++) {
            if (!FD_ISSET(i, &read_fds)) continue;

            if (i == sockfd) {
                // Новое подключение
                clilen = sizeof(cli_addr);
                int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
                if (newsockfd < 0) error("ошибка при принятии соединения");

                // Получение информации о клиенте
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &cli_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                char client_hostname[256] = "Неизвестный хост";
                struct hostent *hst = gethostbyaddr(&cli_addr.sin_addr, 4, AF_INET);
                if (hst) strncpy(client_hostname, hst->h_name, sizeof(client_hostname) - 1);

                // Добавление клиента в массив
                int client_index = -1;
                for (int j = 0; j < FD_SETSIZE; j++) {
                    if (clients[j].sockfd == -1) {
                        client_index = j;
                        break;
                    }
                }
                if (client_index == -1) {
                    fprintf(stderr, "Слишком много клиентов\n");
                    close(newsockfd);
                    continue;
                }

                clients[client_index].sockfd = newsockfd;
                strcpy(clients[client_index].client_ip, client_ip);
                strcpy(clients[client_index].client_hostname, client_hostname);
                clients[client_index].step = 0;
                bzero(clients[client_index].buffer, sizeof(clients[client_index].buffer));

                nclients++;
                printf("+%s [%s] новое подключение!\n", client_hostname, client_ip);
                printusers();

                FD_SET(newsockfd, &master_fds);
                if (newsockfd > fd_max) fd_max = newsockfd;
                client_count++;
            } else {
                // Данные от клиента
                int client_index = -1;
                for (int j = 0; j < FD_SETSIZE; j++) {
                    if (clients[j].sockfd == i) {
                        client_index = j;
                        break;
                    }
                }
                if (client_index == -1) continue;

                Client *client = &clients[client_index];
                int bytes_recv = recv(i, client->buffer, sizeof(client->buffer) - 1, 0);
                if (bytes_recv <= 0) {
                    // Клиент отключился
                    nclients--;
                    printf("-%s [%s] отключение\n", client->client_hostname, client->client_ip);
                    printusers();
                    close(i);
                    FD_CLR(i, &master_fds);
                    client->sockfd = -1;
                    client_count--;
                    continue;
                }
                client->buffer[bytes_recv] = 0;

                // Обработка данных в зависимости от шага
                if (client->step == 0) {
                    // Ожидание операции
                    if (strcmp(client->buffer, "exit") == 0) {
                        char response[] = "Удачи";
                        send(i, response, strlen(response), 0);
                        nclients--;
                        printf("-%s [%s] отключение\n", client->client_hostname, client->client_ip);
                        printusers();
                        close(i);
                        FD_CLR(i, &master_fds);
                        client->sockfd = -1;
                        client_count--;
                    } else if (strlen(client->buffer) == 1 && strchr("+-*/", client->buffer[0])) {
                        client->op = client->buffer[0];
                        client->step = 1;
                    }
                } else if (client->step == 1) {
                    // Ожидание первого числа
                    client->a = atof(client->buffer);
                    client->step = 2;
                } else if (client->step == 2) {
                    // Ожидание второго числа и вычисление
                    client->b = atof(client->buffer);
                    double result = calculate(client->op, client->a, client->b);
                    char response[1024];
                    snprintf(response, sizeof(response), "%.2f", result);
                    send(i, response, strlen(response), 0);
                    client->step = 0;
                }
                bzero(client->buffer, sizeof(client->buffer));
            }
        }
    }

    close(sockfd);
    return 0;
}

// Функция обработки ошибок
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Вывод количества подключенных пользователей
void printusers() {
    if (nclients) {
        printf("%d пользователь(ей) онлайн\n", nclients);
    } else {
        printf("Нет пользователей онлайн\n");
    }
}

// Вычисление результата операции
double calculate(char op, double a, double b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) {
                fprintf(stderr, "Деление на ноль\n");
                return 0;
            }
            return (a / b);
        default: return 0;
    }
}
