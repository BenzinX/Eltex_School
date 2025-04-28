#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <locale.h>
#include <time.h>

int is_in_subnet(const char* ip, const char* gateway, const char* netmask);
void generate_random_ip(char* ip_buffer);

int main(int args, char* argv[]) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // Проверка аргументов командной строки
    if (args != 4) {
        printf("Использование: %s <шлюз> <маска> <количество пакетов>\n", argv[0]);
        return 1;
    }

    const char* gateway = argv[1];
    const char* netmask = argv[2];
    int N = atoi(argv[3]);

    srand(time(NULL));

    int same_subnet = 0;
    int other_subnet = 0;

    printf("Имитация обработки %d пакетов...\n", N);

    for (int i = 0; i < N; i++) {
        char dest_ip[16];
        generate_random_ip(dest_ip);

        if (is_in_subnet(dest_ip, gateway, netmask)) {
            same_subnet++;
            printf("Пакет %d: %s - своя подсеть\n", i + 1, dest_ip);
        }
        else {
            other_subnet++;
            printf("Пакет %d: %s - другая подсеть\n", i + 1, dest_ip);
        }
    }

    // Вывод статистики
    printf("\nСтатистика:\n");
    printf("Пакетов для своей подсети: %d (%.2f%%)\n",
        same_subnet, (float)same_subnet / N * 100);
    printf("Пакетов для других подсетей: %d (%.2f%%)\n",
        other_subnet, (float)other_subnet / N * 100);

    return 0;
}

int is_in_subnet(const char* ip, const char* gateway, const char* netmask) {
    struct in_addr ip_addr, gateway_addr, netmask_addr;

    // Преобразование строк в IP-адреса
    inet_pton(AF_INET, ip, &ip_addr);
    inet_pton(AF_INET, gateway, &gateway_addr);
    inet_pton(AF_INET, netmask, &netmask_addr);

    // Применение маски подсети к IP и шлюзу
    uint32_t ip_network = ip_addr.s_addr & netmask_addr.s_addr;
    uint32_t gateway_network = gateway_addr.s_addr & netmask_addr.s_addr;

    return (ip_network == gateway_network);
}

void generate_random_ip(char* ip_buffer) {
    uint8_t octets[4];
    for (int i = 0; i < 4; i++) {
        octets[i] = rand() % 256;
    }
    snprintf(ip_buffer, 16, "%d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
}
