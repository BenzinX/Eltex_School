#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// Объявления функций
void error(const char *msg);
void printusers();
double calculate(char op, double a, double b);
void dostuff(int sock);

int nclients = 0;

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    printf("TCP Сервер\n");
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <порт>\n", argv[0]);
        exit(1);
    }

    // Создание сокета для TCP-соединения
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ошибка при открытии сокета");

    // Инициализация структуры адреса сервера
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET; // IPv4 протокол
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Привязка ко всем доступным интерфейсам
    // Установка порта сервера (конвертация в сетевой порядок байтов)
    serv_addr.sin_port = htons(portno);

    // Привязка сокета к адресу и порту
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ошибка при привязке сокета");

    // Ожидание входящих соединений (максимум 5 в очереди)
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        // Принятие входящего соединения от клиента
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) error("ошибка при принятии соединения(accept)");

        struct hostent *hst;
        // Получение имени хоста клиента по его IP-адресу
        hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s] новое подключение!\n",
               (hst) ? hst->h_name : "Неизвестный хост",
               inet_ntoa(cli_addr.sin_addr));

        pid = fork();
        if (pid < 0) error("ошибка при создании дочернего процесса");
        if (pid == 0) {
            // Закрытие основного сокета в дочернем процессе
            close(sockfd);
            // Обработка клиентского соединения
            dostuff(newsockfd);
            // Закрытие клиентского сокета
            close(newsockfd);
            exit(0);
        } else {
            // Закрытие клиентского сокета в родительском процессе
            close(newsockfd);
        }
    }

    // Закрытие основного сокета
    close(sockfd);
    return 0;
}

// Функция обработки ошибок при работе с сокетами
void error(const char *msg) {
    perror(msg);
    exit(1);
}

void printusers() {
    if (nclients) {
        printf("%d пользователь(ей) онлайн\n", nclients);
    } else {
        printf("Нет пользователей онлайн\n");
    }
}

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

// Функция обработки взаимодействия с клиентом через сокет
void dostuff(int sock) {
    char buffer[1024];
    int bytes_recv;
    char op;
    double a, b;
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);
    char client_ip[INET_ADDRSTRLEN];
    char client_hostname[256] = "Неизвестный хост";

    // Получение информации о клиенте (IP-адрес и имя хоста)
    if (getpeername(sock, (struct sockaddr*)&cli_addr, &len) == 0) {
        // Конвертация IP-адреса клиента в строковый формат
        inet_ntop(AF_INET, &cli_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        
        // Получение имени хоста клиента по его IP-адресу
        struct hostent *hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        if (hst) {
            strncpy(client_hostname, hst->h_name, sizeof(client_hostname)-1);
            client_hostname[sizeof(client_hostname)-1] = '\0';
        }
    }

    nclients++;
    printusers();

    while (1) {
        // Получение данных от клиента (операция)
        bytes_recv = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_recv <= 0) {
            nclients--;
            printf("-%s [%s] отключение\n", client_hostname, client_ip);
            printusers();
            return;
        }
        buffer[bytes_recv] = 0;

        // Проверка на "exit"
        if (strcmp(buffer, "exit") == 0) {
            bzero(buffer, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "Удачи");
            // Отправка сообщения клиенту перед отключением
            send(sock, buffer, strlen(buffer), 0);
            nclients--;
            printf("-%s [%s] отключение\n", client_hostname, client_ip);
            printusers();
            return;
        }

        // Если не "exit", считаем, что это операция
        op = buffer[0];

        // Получение первого числа от клиента
        bytes_recv = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_recv <= 0) {
            nclients--;
            printf("-%s [%s] отключение\n", client_hostname, client_ip);
            printusers();
            return;
        }
        buffer[bytes_recv] = 0;
        a = atof(buffer);

        // Получение второго числа от клиента
        bytes_recv = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_recv <= 0) {
            nclients--;
            printf("-%s [%s] отключение\n", client_hostname, client_ip);
            printusers();
            return;
        }
        buffer[bytes_recv] = 0;
        b = atof(buffer);

        // Вычисление и отправка результата клиенту
        double result = calculate(op, a, b);
        bzero(buffer, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%.2f", result);
        send(sock, buffer, strlen(buffer), 0);
    }
}
