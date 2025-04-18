#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>


int parse_letter_perm(const char* perm); // Преобразование буквенного представления в битовое
int apply_chmod(int mode, const char* mod); // Учёт изменения прав как в chmod
int get_file_permissions(const char* filename); // Получение прав доступа к файлу
void print_permissions(int mode); // Вывод прав во всех форматах
void print_binary_perm(int mode); // Вывод числа в двоичной форме
int parse_binary_perm(const char* perm); // Преобразование двоичного формата в битовое
int parse_numeric_perm(const char* perm); // ...битового в двоичное

int main() {
    char input[256];
    int mode;

    while (1) {
        printf("\nВыберите действие:\n");
        printf("1. Ввести права доступа\n");
        printf("2. Прочитать права файла\n");
        printf("3. Изменить права доступа\n");
        printf("4. Выход\n");
        printf("Ваш выбор: ");
        fgets(input, sizeof(input), stdin);

        int choice = atoi(input);
        if (choice == 4) break;

        switch (choice) {
        case 1:
            printf("Введите права (буквенные, например rwxr-xr-x, цифровые, например 755, или битовое, например 111101100): ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0; // Удаление \n

            if (strlen(input) == 9 && (input[0] == 'r' || input[0] == '-')) {
                mode = parse_letter_perm(input);
                if (mode == -1) {
                    printf("Неверный формат буквенных прав\n");
                    continue;
                }
            }
            else if (strlen(input) == 3 && isdigit(input[0])) {
                mode = parse_numeric_perm(input);
                if (mode == -1) {
                    printf("Неверный формат цифровых прав\n");
                    continue;
                }
            }
            else if (strlen(input) == 9 && (input[0] == '0' || input[0] == '1')) {
                mode = parse_binary_perm(input);
                if (mode == -1) {
                    printf("Неверный формат битовых прав\n");
                    continue;
                }
            }
            else {
                printf("Неверный формат ввода\n");
                continue;
            }
            print_permissions(mode);
            break;

        case 2:
            printf("Введите имя файла: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            mode = get_file_permissions(input);
            if (mode != -1) {
                print_permissions(mode);
            }
            break;

        case 3:
            printf("Введите текущие права (буквенные, например rwxr-xr-x, цифровые, например 755, или битовое, например 111101100): ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            if (strlen(input) == 9 && (input[0] == 'r' || input[0] == '-')) {
                mode = parse_letter_perm(input);
                if (mode == -1) {
                    printf("Неверный формат буквенных прав\n");
                    continue;
                }
            }
            else if (strlen(input) == 3 && isdigit(input[0])) {
                mode = parse_numeric_perm(input);
                if (mode == -1) {
                    printf("Неверный формат цифровых прав\n");
                    continue;
                }
            }
            else if (strlen(input) == 9 && (input[0] == '0' || input[0] == '1')) {
                mode = parse_binary_perm(input);
                if (mode == -1) {
                    printf("Неверный формат битовых прав\n");
                    continue;
                }
            }
            else {
                printf("Неверный формат ввода\n");
                continue;
            }

            printf("Введите модификацию (например, u+x,g=rw,o-rwx): ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            int new_mode = apply_chmod(mode, input);
            if (new_mode == -1) {
                printf("Неверный формат модификации\n");
                continue;
            }

            printf("Исходные права:\n");
            print_permissions(mode);
            printf("Новые права:\n");
            print_permissions(new_mode);
            break;

        default:
            printf("Неверный выбор\n");
        }
    }

    return 0;
}

// Функция для преобразования буквенного представления в битовое
int parse_letter_perm(const char* perm) {
    if (strlen(perm) != 9) return -1;
    int mode = 0;
    for (int i = 0; i < 9; i += 3) {
        if (perm[i] == 'r') mode |= 0400 >> i;
        if (perm[i + 1] == 'w') mode |= 0200 >> i;
        if (perm[i + 2] == 'x') mode |= 0100 >> i;
    }
    return mode;
}

// Функция для преобразования цифрового представления в битовое
int parse_numeric_perm(const char* perm) {
    if (strlen(perm) != 3) return -1;
    int mode = 0;
    for (int i = 0; i < 3; i++) {
        if (!isdigit(perm[i])) return -1;
        int val = perm[i] - '0';
        if (val > 7) return -1;
        mode = (mode << 3) | val;
    }
    return mode;
}

// Функция для преобразования двоичного представления в битовое
int parse_binary_perm(const char* perm) {
    if (strlen(perm) != 9) return -1;
    int mode = 0;
    for (int i = 0; i < 9; i++) {
        if (perm[i] != '0' && perm[i] != '1') return -1;
        mode = (mode << 1) | (perm[i] - '0');
    }
    return mode;
}

// Функция для вывода битового представления в двоичной форме
void print_binary_perm(int mode) {
    char binary[10] = "000000000";
    for (int i = 8; i >= 0; i--) {
        binary[8 - i] = (mode & (1 << i)) ? '1' : '0';
    }
    printf("Битовое: %s\n", binary);
}

// Функция для вывода прав доступа в трех форматах
void print_permissions(int mode) {
    char letter[10] = "---------";
    // Владелец
    if (mode & 0400) letter[0] = 'r';
    if (mode & 0200) letter[1] = 'w';
    if (mode & 0100) letter[2] = 'x';
    // Группа
    if (mode & 0040) letter[3] = 'r';
    if (mode & 0020) letter[4] = 'w';
    if (mode & 0010) letter[5] = 'x';
    // Остальные
    if (mode & 0004) letter[6] = 'r';
    if (mode & 0002) letter[7] = 'w';
    if (mode & 0001) letter[8] = 'x';

    printf("Буквенное: %s\n", letter);
    printf("Цифровое: %o\n", mode);
    print_binary_perm(mode);
}

// Функция для получения прав доступа файла
int get_file_permissions(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        perror("Ошибка при получении информации о файле");
        return -1;
    }
    return st.st_mode & 0777; // Маска для прав доступа
}

// Функция для применения модификации прав (chmod-стиль)
int apply_chmod(int mode, const char* mod) {
    int new_mode = mode;
    char* mod_copy = strdup(mod);
    char* token = strtok(mod_copy, ",");

    while (token) {
        int who = 0; // 0=u, 1=g, 2=o, 3=ugo
        int i = 0;

        // Определение "кого" затрагивает команда
        while (token[i] == 'u' || token[i] == 'g' || token[i] == 'o') {
            if (token[i] == 'u') who |= 1;
            else if (token[i] == 'g') who |= 2;
            else if (token[i] == 'o') who |= 4;
            i++;
        }
        if (who == 0) who = 7; // Если задаем +[rwx], т.е. не указываем первый параметр, то ugo

        // Пропускаем пробелы
        while (token[i] == ' ') i++;

        // Обработка операции и прав
        char op = token[i++];
        if (op != '+' && op != '-' && op != '=') {
            free(mod_copy);
            return -1;
        }

        int perm = 0;
        while (token[i] == 'r' || token[i] == 'w' || token[i] == 'x') {
            if (token[i] == 'r') perm |= 4;
            else if (token[i] == 'w') perm |= 2;
            else if (token[i] == 'x') perm |= 1;
            i++;
        }

        // Применение изменений
        for (int j = 0; j < 3; j++) {
            if (who & (1 << j)) {
                int shift = (2 - j) * 3;
                int mask = 7 << shift;

                if (op == '+') new_mode |= (perm << shift);
                else if (op == '-') new_mode &= ~(perm << shift);
                else if (op == '=') {
                    new_mode &= ~mask;
                    new_mode |= (perm << shift);
                }
            }
        }
        token = strtok(NULL, ",");
    }
    free(mod_copy);
    return new_mode;
}

