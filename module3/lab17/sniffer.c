#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#define BUFLEN 65535

static int sockfd;

void write_to_log(const char *src_ip, uint16_t src_port, 
                 const char *dst_ip, uint16_t dst_port,
                 const char *message, const char *filename);
void handle_sigint(int sig __attribute__((unused)));

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <интерфейс> <порт сервера>\n", argv[0]);
        exit(1);
    }

    int server_port = atoi(argv[2]);
    if (server_port <= 0 || server_port > 65535) {
        fprintf(stderr, "Неверный порт: %s\n", argv[2]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("Ошибка создания сокета");
        exit(1);
    }

    signal(SIGINT, handle_sigint);

    char buffer[BUFLEN];
    struct sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);

    printf("Сниффер запущен, захват UDP-пакетов на порт %d...\n", server_port);

    while (1) {
        int len = recvfrom(sockfd, buffer, BUFLEN, 0, (struct sockaddr*)&src_addr, &addr_len);
        if (len < 0) {
            perror("Ошибка получения пакета");
            continue;
        }

        struct ip *ip_header = (struct ip*)buffer;
        int ip_header_len = ip_header->ip_hl * 4;
        if (ip_header->ip_p != IPPROTO_UDP) continue;

        struct udphdr *udp_header = (struct udphdr*)(buffer + ip_header_len);
        if (ntohs(udp_header->uh_dport) != server_port) continue;

        char *payload = buffer + ip_header_len + 8;
        int payload_len = ntohs(udp_header->uh_ulen) - 8;

        char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip_header->ip_src, src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &ip_header->ip_dst, dst_ip, INET_ADDRSTRLEN);

        printf("Пакет: %s:%d -> %s:%d, Длина данных: %d\n",
               src_ip, ntohs(udp_header->uh_sport),
               dst_ip, ntohs(udp_header->uh_dport),
               payload_len);

        if (payload_len > 0) {
            char *message = malloc(payload_len + 1);
            memcpy(message, payload, payload_len);
            message[payload_len] = '\0';
            printf("Сообщение: %s\n", message);
            write_to_log(src_ip, ntohs(udp_header->uh_sport),
                        dst_ip, ntohs(udp_header->uh_dport),
                        message, "sniffed_messages.txt");
            free(message);
        }
    }

    close(sockfd);
    return 0;
}

void handle_sigint(int sig __attribute__((unused))) {
    printf("\nЗавершение работы сниффера...\n");
    close(sockfd);
    exit(0);
}

void write_to_log(const char *src_ip, uint16_t src_port, 
                 const char *dst_ip, uint16_t dst_port,
                 const char *message, const char *filename) {
    FILE *text_file = fopen(filename, "a");
    if (text_file) {
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str)-1] = '\0';
        fprintf(text_file, "[%s] %s:%d -> %s:%d: %s\n",
                time_str, src_ip, src_port, dst_ip, dst_port, message);
        fclose(text_file);
    }
}
