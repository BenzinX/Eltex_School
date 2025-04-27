#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <time.h>

#define SNAPLEN 65535
#define PROMISC 1
#define TIMEOUT 1000

/* Прототипы функций */
// Функция для обработки пакетов
void process_packet(u_char *user, const struct pcap_pkthdr *header, const u_char *packet);
// Запись перехваченного файла
void write_to_log(const char *src_ip, uint16_t src_port, 
                 const char *dst_ip, uint16_t dst_port,
                 const char *message, const char *filename);

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
    pcap_t *handle;
    struct bpf_program fp;
    char filter_exp[100];
    sprintf(filter_exp, "udp dst port %d", server_port);

    /* 
     * Открытие сетевого интерфейса для захвата:
     * pcap_open_live() - основная функция для захвата пакетов
     * Параметры:
     *   - имя интерфейса
     *   - максимальная длина пакета (snaplen)
     *   - promiscuous mode (1 - включен)
     *   - timeout в миллисекундах
     *   - буфер для ошибок
     */
    handle = pcap_open_live(dev, SNAPLEN, PROMISC, TIMEOUT, errbuf);
    if (!handle) {
        fprintf(stderr, "Ошибка открытия интерфейса %s: %s\n", dev, errbuf);
        exit(1);
    }

    /*
     * Компиляция BPF фильтра:
     * pcap_compile() преобразует текстовый фильтр в BPF код
     * Параметры:
     *   - handle
     *   - указатель на структуру bpf_program
     *   - строка фильтра
     *   - optimize (0 - выключено)
     *   - netmask (PCAP_NETMASK_UNKNOWN - неизвестен)
     */
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Ошибка компиляции фильтра: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        exit(1);
    }

    /*
     * Установка фильтра:
     * pcap_setfilter() применяет скомпилированный фильтр
     */
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Ошибка установки фильтра: %s\n", pcap_geterr(handle));
        pcap_freecode(&fp);
        pcap_close(handle);
        exit(1);
    }
    pcap_freecode(&fp); // Освобождаем память фильтра

    /*
     * Открытие файла для сохранения дампа пакетов:
     * pcap_dump_open() создает файл в формате pcap
     * Формат pcap поддерживается Wireshark и tcpdump
     */
    pcap_dumper_t *dump_file = pcap_dump_open(handle, "sniffed_packets.pcap");
    if (!dump_file) {
        fprintf(stderr, "Ошибка открытия дамп-файла: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        exit(1);
    }

    printf("Сниффер запущен на %s, захват UDP-пакетов на порт %d...\n", dev, server_port);

    /*
     * Основной цикл захвата пакетов:
     * pcap_loop() обрабатывает пакеты в бесконечном цикле
     * Параметры:
     *   - handle
     *   - количество пакетов (0 - бесконечно)
     *   - callback функция для обработки
     *   - пользовательские данные (передаются в callback)
     */
    pcap_loop(handle, 0, process_packet, (u_char *)dump_file);

    // Закрытие ресурсов
    pcap_dump_close(dump_file);
    pcap_close(handle);
    return 0;
}

/*
 * Функция записи сообщения в лог-файл
 */
void write_to_log(const char *src_ip, uint16_t src_port, 
                 const char *dst_ip, uint16_t dst_port,
                 const char *message, const char *filename) {
    FILE *text_file = fopen(filename, "a");
    if (text_file) {
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str)-1] = '\0'; // Удаляем \n
        
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
    ip_header_len = ip_header->ip_hl * 4; // Длина IP-заголовка в 32-битных словах

    // Проверяем, что это UDP-пакет
    if (ip_header->ip_p != IPPROTO_UDP) return;

    // Разбираем UDP-заголовок
    udp_header = (struct udphdr *)(packet + 14 + ip_header_len);
    payload = packet + 14 + ip_header_len + 8; // UDP заголовок всегда 8 байт
    payload_len = ntohs(udp_header->uh_ulen) - 8; // Длина данных

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
        
        // Записываем в лог-файл
        write_to_log(src_ip, ntohs(udp_header->uh_sport),
                    dst_ip, ntohs(udp_header->uh_dport),
                    message, "sniffed_messages.txt");
        
        free(message);
    }

    /*
     * Запись пакета в дамп-файл:
     * pcap_dump() сохраняет пакет в файл в формате pcap
     */
    pcap_dump((u_char *)dump_file, header, packet);
}
