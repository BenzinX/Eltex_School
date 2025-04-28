#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Добавлен заголовочный файл для malloc и free

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s строка1 [строка2 ...]\n", argv[0]);
        return 1;
    }

    // Подсчитываем общую длину строки
    size_t total_len = 0;
    for (int i = 1; i < argc; i++) {
        total_len += strlen(argv[i]);
    }
    total_len += argc - 1; // Пробелы между словами

    // Выделяем память под результирующую строку
    char *result = malloc(total_len + 1);
    if (!result) {
        perror("Ошибка выделения памяти");
        return 1;
    }

    // Склеиваем строки
    result[0] = '\0';
    for (int i = 1; i < argc; i++) {
        strcat(result, argv[i]);
        if (i < argc - 1) {
            strcat(result, " ");
        }
    }

    printf("Результат: %s\n", result);
    free(result); // Освобождаем память
    return 0;
}
