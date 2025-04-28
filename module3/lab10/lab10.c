#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>

sem_t *file_sem;      // Семафор для доступа к файлу
sem_t *reader_done_sem; // Семафор для сигнала завершения чтения
sem_t *writer_done_sem; // Семафор для сигнала завершения записи

void print_final_file() {
    printf("\n=== Итоговое содержимое файла ===\n");
    FILE *file = fopen("output.txt", "r");
    if (file != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
        }
        fclose(file);
    } else {
        printf("Файл не существует или не может быть открыт\n");
    }
    printf("================================\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <количество_чисел>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_numbers = atoi(argv[1]);
    if (num_numbers <= 0) {
        fprintf(stderr, "Количество чисел должно быть положительным\n");
        exit(EXIT_FAILURE);
    }

    // Создание пустого файла перед началом
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        perror("Ошибка создания файла");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    // Инициализация именованных семафоров
    file_sem = sem_open("/file_sem", O_CREAT | O_EXCL, 0644, 1);
    reader_done_sem = sem_open("/reader_done_sem", O_CREAT | O_EXCL, 0644, 0);
    writer_done_sem = sem_open("/writer_done_sem", O_CREAT | O_EXCL, 0644, 0);
    if (file_sem == SEM_FAILED || reader_done_sem == SEM_FAILED || writer_done_sem == SEM_FAILED) {
        perror("Ошибка создания семафоров");
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
        close(pipefd[0]);

        srand(time(NULL) ^ getpid());
        for (int i = 0; i < num_numbers; i++) {
            if (i > 0) {
                sem_wait(writer_done_sem); // Ждать завершения записи родителем
            }

            sem_wait(file_sem); // Блокировка для чтения

            FILE *file = fopen("output.txt", "r");
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
            } else {
                printf("(файл не существует)\n");
            }

            sem_post(file_sem); // Разблокировка после чтения

            int num = rand() % 100;
            write(pipefd[1], &num, sizeof(num));

            sem_post(reader_done_sem); // Сигнал, что чтение завершено
        }

        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    } else { // Родительский процесс
        close(pipefd[1]);

        file = fopen("output.txt", "w");
        if (file == NULL) {
            perror("Ошибка открытия файла");
            exit(EXIT_FAILURE);
        }

        int num;
        for (int i = 0; i < num_numbers; i++) {
            sem_wait(reader_done_sem); // Ожидание завершения чтения дочерним процессом

            if (read(pipefd[0], &num, sizeof(num)) <= 0) {
                perror("Ошибка чтения из канала");
                exit(EXIT_FAILURE);
            }

            sem_wait(file_sem); // Блокировка для записи

            printf("[Шаг %d] Родитель получил число: %d\n\n", i + 1, num);
            fprintf(file, "%d\n", num);
            fflush(file);

            sem_post(file_sem); // Разблокировка после записи

            sem_post(writer_done_sem); // Сигнал, что запись завершена
        }

        fclose(file);
        close(pipefd[0]);
        wait(NULL);

        // Очистка семафоров
        sem_close(file_sem);
        sem_close(reader_done_sem);
        sem_close(writer_done_sem);
        sem_unlink("/file_sem");
        sem_unlink("/reader_done_sem");
        sem_unlink("/writer_done_sem");

        // Вывод итогового содержимого файла
        print_final_file();
    }

    return EXIT_SUCCESS;
}
