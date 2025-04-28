#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

#define SNAPLEN 65535
#define PROMISC 1
#define TIMEOUT 1000

// Глобальные переменные для доступа в обработчике сигнала
static pcap_t *handle;
static pcap_dumper_t *dump_file;

// Прототипы функций
void process_packet(u_char *user, const struct pcap_pkthdr *header, const u_char *packet);
void write_to_log(const char *src_ip, uint16_t src_port, 
                 const char *dst_ip, uint16_t dst_port,
                 const char *message, const char *filename);
void handle_sigint();

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <интерфейс> <порт сервера>\n", argv[0]);
        exit(1);
    }

    char *dev = argv[1];
    int server_port = atoi(argv[2]);
    if (server_port <= 0 || server_port > 65535) {
        fprintf(stderr, "Неверный порт: %s\n", argv[2]);
        exit(1);
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    char filter_exp[100];
    snprintf(filter_exp, sizeof(filter_exp), "udp dst port %d", server_port);

    // Открытие сетевого интерфейса для захвата
    handle = pcap_open_live(dev, SNAPLEN, PROMISC, TIMEOUT, errbuf);
    if (!handle) {
        fprintf(stderr, "Ошибка открытия интерфейса %s: %s\n", dev, errbuf);
        exit(1);
    }

    // Компиляция BPF фильтра
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Ошибка компиляции фильтра: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        exit(1);
    }

    // Установка фильтра
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Ошибка установки фильтра: %s\n", pcap_geterr(handle));
        pcap_freecode(&fp);
        pcap_close(handle);
        exit(1);
    }
    pcap_freecode(&fp);

    // Открытие файла для дампа пакетов
    dump_file = pcap_dump_open(handle, "sniffed_packets.pcap");
    if (!dump_file) {
        fprintf(stderr, "Ошибка открытия дамп-файла: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        exit(1);
    }

    // Установка обработчика SIGINT
    signal(SIGINT, handle_sigint);

    printf("Сниффер запущен на %s, захват UDP-пакетов на порт %d...\n", dev, server_port);

    // Основной цикл захвата пакетов
    pcap_loop(handle, 0, process_packet, (u_char *)dump_file);

    // Закрытие ресурсов
    pcap_dump_close(dump_file);
    pcap_close(handle);
    printf("Сниффер завершён.\n");
    return 0;
}

// Обработчик SIGINT для Ctrl+C
void handle_sigint() {
    printf("\nЗавершение работы сниффера...\n");
    pcap_breakloop(handle); // Прерывает pcap_loop
}

// Функция записи сообщения в лог-файл
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

// Функция для обработки пакетов
void process_packet(u_char *user, const struct pcap_pkthdr *header, const u_char *packet) {
    pcap_dumper_t *dump_file = (pcap_dumper_t *)user;
    struct ip *ip_header;
    struct udphdr *udp_header;
    const u_char *payload;
    int ip_header_len, payload_len;

    // Разбираем IP-заголовок (пропускаем Ethernet-заголовок 14 байт)
    ip_header = (struct ip *)(packet + 14);
    ip_header_len = ip_header->ip_hl * 4;

    // Проверяем, что это UDP-пакет
    if (ip_header->ip_p != IPPROTO_UDP) return;

    // Разбираем UDP-заголовок
    udp_header = (struct udphdr *)(packet + 14 + ip_header_len);
    payload = packet + 14 + ip_header_len + 8;
    payload_len = ntohs(udp_header->uh_ulen) - 8;

    // Преобразуем IP-адреса в строки
    char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_header->ip_src, src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip_header->ip_dst, dst_ip, INET_ADDRSTRLEN);

    printf("Пакет: %s:%d -> %s:%d, Длина данных: %d\n",
           src_ip, ntohs(udp_header->uh_sport),
           dst_ip, ntohs(udp_header->uh_dport),
           payload_len);

    // Обработка полезной нагрузки
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

    // Запись пакета в дамп-файл
    pcap_dump((u_char *)dump_file, header, packet);
}
