#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <количество_случайных_чисел>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_numbers = atoi(argv[1]);
    if (num_numbers <= 0) {
        fprintf(stderr, "Количество случайных чисел должно быть положительным\n");
        exit(EXIT_FAILURE);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Дочерний процесс
        close(pipefd[0]); // Закрываем чтение

        srand(time(NULL) ^ getpid());
        for (int i = 0; i < num_numbers; i++) {
            int num = rand() % 100; // Генерация числа от 0 до 99
            write(pipefd[1], &num, sizeof(num));
        }

        close(pipefd[1]); // Закрываем запись
        exit(EXIT_SUCCESS);
    } else { // Родительский процесс
        close(pipefd[1]); // Закрываем запись

        FILE *file = fopen("output.txt", "w");
        if (file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        int num;
        ssize_t bytes_read;
        while ((bytes_read = read(pipefd[0], &num, sizeof(num))) > 0) {
            printf("Получено число: %d\n", num);
            fprintf(file, "%d\n", num);
        }
        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }


        fclose(file);
        close(pipefd[0]); // Закрываем чтение
        wait(NULL); // Ожидаем завершения дочернего процесса
    }

    return EXIT_SUCCESS;
}
