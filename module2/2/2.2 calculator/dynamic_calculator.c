#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>

// Структура команды
typedef struct {
    char name[16];
    char description[64];
    double (*func) (double, double);
} Command;

// Прототипы функций
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);
double power(double a, double b);
void show_help(Command* commands, int count);
void clear_input_buffer();

int main() {
    setlocale(LC_ALL, "Russian");
    Command commands[] = {
        {"add", "сложение", add},
        {"sub", "вычитание", subtract},
        {"mul", "умножение", multiply},
        {"div", "деление", divide},
        {"pow", "возведение в степень", power}
    };
    int command_count = sizeof(commands) / sizeof(Command);

    show_help(commands, command_count);

    char input[32];
    double a, b;

    while (1) {
        printf("> ");
        if (scanf_s("%s", input, (unsigned)sizeof(input)) != 1) {
            clear_input_buffer();
            continue;
        }

        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "help") == 0) {
            show_help(commands, command_count);
            continue;
        }

        // Поиск команды
        int found = 0;
        for (int i = 0; i < command_count; i++) {
            if (strcmp(input, commands[i].name) == 0) {
                found = 1;
                printf_s("Введите a и b: ");
                if ((scanf_s("%lf %lf", &a, &b)) == 2) {
                    double res = commands[i].func(a, b);
                    printf("Рузальтат = %lf\n", res);
                }
                else {
                    clear_input_buffer();
                    printf_s("Введите два числа!\n");
                }
                break;
            }
        }

        if (found == 0) {
            printf_s("Неизвестная команда. Введите 'help'.\n");
            clear_input_buffer();
        }
    }

    return 0;
}

// Реализация операций
double add(double a, double b) { return a + b; }
double subtract(double a, double b) { return a - b; }
double multiply(double a, double b) { return a * b; }
double divide(double a, double b) {
    if (b == 0) {
        fprintf(stderr, "Ошибка: деление на ноль!\n");
        return NAN;
    }
    return a / b;
}
double power(double a, double b) { return pow(a, b); }

// Вывод списка команд
void show_help(Command* commands, int count) {
    printf("Доступные команды:\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s - %s\n", i + 1, commands[i].name, commands[i].description);
    }
    printf("\n   help - показать справку\n");
    printf("   exit - выход\n");
}

void clear_input_buffer() {
    while (getchar() != '\n');
}
