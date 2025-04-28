#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 256
#define MAX_ARGS 32

// Функция для разделения строки на аргументы
void parse_input(char *input, char *args[]) {
    int i = 0;
    char *token = strtok(input, " \n");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL; // Последний элемент должен быть NULL для execvp
}

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        // Вывод приглашения
        printf("myshell> ");
        fflush(stdout);

        // Чтение команды
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\nВыход\n");
            break;
        }

        // Если строка пустая, пропускаем
        if (input[0] == '\n') {
            continue;
        }

        // Разделяем входную строку на аргументы
        parse_input(input, args);

        // Проверка на команду выхода
        if (strcmp(args[0], "exit") == 0) {
            printf("Выход\n");
            break;
        }

        // Создаем дочерний процесс
        pid_t pid = fork();
        if (pid < 0) {
            perror("Ошибка fork");
            continue;
        } else if (pid == 0) {
            // Дочерний процесс
            if (execvp(args[0], args) == -1) {
                perror("Ошибка выполнения команды");
                exit(1);
            }
        } else {
            // Родительский процесс
            wait(NULL); // Ждем завершения дочернего процесса
        }
    }

    return 0;
}
