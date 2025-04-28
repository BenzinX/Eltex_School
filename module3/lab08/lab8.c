#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <time.h>
#include <fcntl.h>

int semid; // Идентификатор семафора

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

void reader_lock() {
    int sem_value = semctl(semid, 0, GETVAL);
    if (sem_value == -1) {
        perror("Ошибка получения значения семафора");
        exit(EXIT_FAILURE);
        
    }

    if (sem_value == 1) {
        struct sembuf op = {0, -1, 0}; // Блокировка для чтения
        if (semop(semid, &op, 1) == -1) {
            perror("Ошибка блокировки чтения");
            exit(EXIT_FAILURE);
        }
    }
}

void reader_unlock() {
    int sem_value = semctl(semid, 0, GETVAL);
    if (sem_value == -1) {
        perror("Ошибка получения значения семафора");
        exit(EXIT_FAILURE);
        
    }
    if (sem_value == 0) {
        struct sembuf op = {0, 1, 0}; // Разблокировка после чтения
        if (semop(semid, &op, 1) == -1) {
            perror("Ошибка разблокировки чтения");
            exit(EXIT_FAILURE);
        }
    }
}

void writer_lock() {
    int sem_value = semctl(semid, 0, GETVAL);
    if (sem_value == -1) {
        perror("Ошибка получения значения семафора");
        exit(EXIT_FAILURE);
    }

    if (sem_value == 1) {
        struct sembuf op = {0, -1, 0}; // Блокировка для записи
        if (semop(semid, &op, 1) == -1) {
            perror("Ошибка блокировки записи");
            exit(EXIT_FAILURE);
        }
    }
}

void writer_unlock() {
    int sem_value = semctl(semid, 0, GETVAL);
    if (sem_value == -1) {
        perror("Ошибка получения значения семафора");
        exit(EXIT_FAILURE);
    }

    if (sem_value == 0) {
        struct sembuf op = {0, 1, 0}; // Разблокировка после записи
        if (semop(semid, &op, 1) == -1) {
            perror("Ошибка разблокировки записи");
            exit(EXIT_FAILURE);
        }
    }
}

void wait_reader() {
    struct sembuf op = {1, -1, 0}; // Ожидание завершения чтения
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка ожидания читателя");
        exit(EXIT_FAILURE);
    }
}

void signal_reader_done() {
    struct sembuf op = {1, 1, 0}; // Сигнал завершения чтения
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка сигнала читателя");
        exit(EXIT_FAILURE);
    }
}

void wait_writer() {
    struct sembuf op = {2, -1, 0}; // Ожидание завершения записи
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка ожидания записи");
        exit(EXIT_FAILURE);
    }
}

void signal_writer_done() {
    struct sembuf op = {2, 1, 0}; // Сигнал завершения записи
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка сигнала завершения записи");
        exit(EXIT_FAILURE);
    }
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

    // Создание семафоров: 0 - для доступа к файлу, 1 - для синхронизации чтения, 2 - для синхронизации записи
    semid = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("Ошибка создания семафора");
        exit(EXIT_FAILURE);
    }

    // Инициализация семафоров
    if (semctl(semid, 0, SETVAL, 1) == -1) { // Доступ к файлу
        perror("Ошибка инициализации семафора доступа");
        exit(EXIT_FAILURE);
    }
    if (semctl(semid, 1, SETVAL, 0) == -1) { // Счетчик чтения
        perror("Ошибка инициализации семафора чтения");
        exit(EXIT_FAILURE);
    }
    if (semctl(semid, 2, SETVAL, 0) == -1) { // Счетчик записи
        perror("Ошибка инициализации семафора записи");
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
                wait_writer(); // Ждать завершения записи родителем
            }

            reader_lock();

            FILE *file = fopen("output.txt", "r");
            printf("\n[Шаг %d] Дочерний процесс читает файл:\n", i+1);
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

            reader_unlock();

            int num = rand() % 100;
            write(pipefd[1], &num, sizeof(num));

            signal_reader_done(); // Сигнал, что чтение завершено
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
            wait_reader(); // Ожидание завершения чтения дочерним процессом

            if (read(pipefd[0], &num, sizeof(num)) <= 0) {
                perror("Ошибка чтения из канала");
                exit(EXIT_FAILURE);
            }

            writer_lock();

            printf("[Шаг %d] Родитель получил число: %d\n\n", i+1, num);
            fprintf(file, "%d\n", num);
            fflush(file);

            writer_unlock();

            signal_writer_done(); // Сигнал, что запись завершена
        }

        fclose(file);
        close(pipefd[0]);
        wait(NULL);

        // Удаление семафора
        if (semctl(semid, 0, IPC_RMID) == -1) {
            perror("Ошибка удаления семафора");
        }

        // Вывод итогового содержимого файла
        print_final_file();
    }

    return EXIT_SUCCESS;
}
