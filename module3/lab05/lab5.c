#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t file_locked = 0;

void handwritten_sigusr1();
void handwritten_sigusr2();
void print_final_file();

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <количество_чисел>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_numbers = atoi(argv[1]);
    if (num_numbers <= 0) {
        fprintf(stderr, "Количество чисел должно быть положительным\n");
        exit(EXIT_FAILURE);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Ошибка создания канала");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Ошибка создания процесса");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Дочерний процесс
        signal(SIGUSR1, handwritten_sigusr1);
        signal(SIGUSR2, handwritten_sigusr2);

        close(pipefd[0]);

        srand(time(NULL) ^ getpid());
        for (int i = 0; i < num_numbers; i++) {
            while (file_locked) {
                pause();
            }

            FILE* file = fopen("output.txt", "r");
            printf("\n[Шаг %d] Дочерний процесс читает файл:\n", i + 1);
            if (file != NULL) {
                char line[256];
                int empty = 1;
                while (fgets(line, sizeof(line), file)) {
                    printf("%s", line);
                    empty = 0;
                }
                if (empty) {
                    printf("(файл пуст)\n");
                }
                fclose(file);
            }
            else {
                printf("(файл не существует)\n");
            }

            int num = rand() % 100;
            write(pipefd[1], &num, sizeof(num));
        }

        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }
    else { // Родительский процесс
        close(pipefd[1]);

        FILE* file = fopen("output.txt", "w");
        if (file == NULL) {
            perror("Ошибка открытия файла");
            exit(EXIT_FAILURE);
        }

        int num;
        for (int i = 0; i < num_numbers; i++) {
            read(pipefd[0], &num, sizeof(num));

            kill(pid, SIGUSR1);

            printf("[Шаг %d] Родитель получил число: %d\n\n", i + 1, num);
            fprintf(file, "%d\n", num);
            fflush(file);

            kill(pid, SIGUSR2);
        }

        fclose(file);
        close(pipefd[0]);
        wait(NULL);

        // Вывод итогового содержимого файла
        print_final_file();
    }

    return EXIT_SUCCESS;
}

void handwritten_sigusr1() {
    file_locked = 1;
}

void handwritten_sigusr2() {
    file_locked = 0;
}

void print_final_file() {
    printf("\n=== Итоговое содержимое файла ===\n");
    FILE* file = fopen("output.txt", "r");
    if (file != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
        }
        fclose(file);
    }
    else {
        printf("Файл не существует или не может быть открыт\n");
    }
    printf("================================\n");
}


