#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>

#define OUTPUT "output.txt"
#define PROCESS_COUNT 3
#define PERMISSIONS 0644
#define SEM_FILE "/file_access"
#define SEM_READER "/reader_signal"
#define SEM_WRITER "/writer_signal"

void display_final_content() {
    printf("\n=== Итоговое содержимое файла ===\n");
    FILE *file = fopen(OUTPUT, "r");
    if (file) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file)) {
            printf("%s", buffer);
        }
        fclose(file);
    } else {
        printf("Не удалось открыть файл\n");
    }
    printf("================================\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Ошибка: укажите количество чисел\n");
        return 1;
    }

    int total_numbers = atoi(argv[1]);
    if (total_numbers <= 0) {
        fprintf(stderr, "Ошибка: количество должно быть положительным\n");
        return 1;
    }

    // Инициализация канала
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        fprintf(stderr, "Ошибка создания канала\n");
        return 1;
    }

    // Создание семафоров
    sem_t *file_sem = sem_open(SEM_FILE, O_CREAT | O_EXCL, PERMISSIONS, 1);
    sem_t *reader_sem = sem_open(SEM_READER, O_CREAT | O_EXCL, PERMISSIONS, 0);
    sem_t *writer_sem = sem_open(SEM_WRITER, O_CREAT | O_EXCL, PERMISSIONS, 0);
    if (file_sem == SEM_FAILED || reader_sem == SEM_FAILED || writer_sem == SEM_FAILED) {
        fprintf(stderr, "Ошибка создания семафоров\n");
        return 1;
    }

    // Создание пустого файла
    FILE *file = fopen(OUTPUT, "w");
    if (!file) {
        fprintf(stderr, "Ошибка инициализации файла\n");
        return 1;
    }
    fclose(file);

    // Распределение чисел между процессами
    int numbers_per_process = total_numbers / PROCESS_COUNT;
    int extra_numbers = total_numbers % PROCESS_COUNT;

    pid_t process_ids[PROCESS_COUNT];
    for (int i = 0; i < PROCESS_COUNT; i++) {
        process_ids[i] = fork();
        if (process_ids[i] == -1) {
            fprintf(stderr, "Ошибка создания процесса\n");
            return 1;
        }

        if (process_ids[i] == 0) { // Дочерний процесс
            close(pipe_fd[0]);
            srand(time(NULL) ^ (getpid()));
            int numbers_to_generate = numbers_per_process + (i < extra_numbers ? 1 : 0);

            for (int j = 0; j < numbers_to_generate; j++) {
                if (j > 0) {
                    sem_wait(writer_sem); // Ожидание завершения записи
                }

                // Чтение файла
                sem_wait(file_sem);
                printf("\n[Процесс %d, Шаг %d] Чтение файла:\n", i + 1, j + 1);
                file = fopen(OUTPUT, "r");
                if (file) {
                    char line[256];
                    int is_empty = 1;
                    while (fgets(line, sizeof(line), file)) {
                        printf("%s", line);
                        is_empty = 0;
                    }
                    if (is_empty) {
                        printf("(файл пуст)\n");
                    }
                    fclose(file);
                } else {
                    fprintf(stderr, "Ошибка чтения файла процессом %d\n", i);
                }
                sem_post(file_sem);

                // Генерация и отправка числа
                int random_num = rand() % 100;
                if (write(pipe_fd[1], &random_num, sizeof(random_num)) == -1) {
                    fprintf(stderr, "Ошибка записи в канал процессом %d\n", i);
                    exit(1);
                }
                sem_post(reader_sem); // Сигнал о завершении чтения
            }
            close(pipe_fd[1]);
            exit(0);
        }
    }

    // Родительский процесс
    close(pipe_fd[1]);
    int received_num;
    for (int i = 0; i < total_numbers; i++) {
        sem_wait(reader_sem); // Ожидание сигнала от читателя
        if (read(pipe_fd[0], &received_num, sizeof(received_num)) == -1) {
            fprintf(stderr, "Ошибка чтения из канала\n");
            break;
        }

        sem_wait(file_sem);
        printf("[Шаг %d] Родитель получил число: %d\n", i + 1, received_num);
        file = fopen(OUTPUT, "a");
        if (file) {
            fprintf(file, "%d\n", received_num);
            fclose(file);
        } else {
            fprintf(stderr, "Ошибка записи в файл\n");
        }
        sem_post(file_sem);
        sem_post(writer_sem); // Сигнал о завершении записи
    }
    close(pipe_fd[0]);

    // Ожидание завершения всех процессов
    for (int i = 0; i < PROCESS_COUNT; i++) {
        wait(NULL);
    }

    // Вывод итогового содержимого
    display_final_content();

    // Очистка семафоров
    sem_close(file_sem);
    sem_close(reader_sem);
    sem_close(writer_sem);
    sem_unlink(SEM_FILE);
    sem_unlink(SEM_READER);
    sem_unlink(SEM_WRITER);

    return 0;
}
