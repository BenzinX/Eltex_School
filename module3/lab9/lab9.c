#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <time.h>
#include <fcntl.h>

#define NUM_READERS 3 // Количество дочерних процессов-читателей
#define RULES 0666 // Права доступа

int semid; // Идентификатор семафора

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


void lock_semaphore() {
    // Диагностическая проверка значения семафора (не влияет на логику)
    int sem_value = semctl(semid, 0, GETVAL);
    if (sem_value == -1) {
        perror("Ошибка получения значения семафора");
        exit(EXIT_FAILURE);
    }

    struct sembuf op = { 0, -1, 0 }; // Блокировка семафора
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка блокировки семафора");
        exit(EXIT_FAILURE);
    }
}

void unlock_semaphore() {
    // Диагностическая проверка значения семафора (не влияет на логику)
    int sem_value = semctl(semid, 0, GETVAL);
    if (sem_value == -1) {
        perror("Ошибка получения значения семафора");
        exit(EXIT_FAILURE);
    }

    struct sembuf op = { 0, 1, 0 }; // Разблокировка семафора
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка разблокировки семафора");
        exit(EXIT_FAILURE);
    }
}


void wait_all_readers() {
    struct sembuf op = { 1, -NUM_READERS, 0 }; // Семафор для синхронизации читателей
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка ожидания читателей");
        exit(EXIT_FAILURE);
    }
}

void signal_reader_done() {
    struct sembuf op = { 1, 1, 0 }; // Семафор для сигнала завершения читателя
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка сигнала читателя");
        exit(EXIT_FAILURE);
    }
}

void wait_writer() {
    struct sembuf op = { 2, -1, 0 }; // Семафор для ожидания записи
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка ожидания записи");
        exit(EXIT_FAILURE);
    }
}

void signal_writer_done() {
    struct sembuf op = { 2, 1, 0 }; // Семафор для сигнала завершения записи
    if (semop(semid, &op, 1) == -1) {
        perror("Ошибка сигнала завершения записи");
        exit(EXIT_FAILURE);
    }
}

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

    // Создание пустого файла перед началом
    FILE* file = fopen("output.txt", "w");
    if (file == NULL) {
        perror("Ошибка создания файла");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    // Создание семафоров: 0 - для доступа к файлу, 1 - для синхронизации читателей, 2 - для синхронизации записи
    semid = semget(IPC_PRIVATE, 3, IPC_CREAT | RULES);
    if (semid == -1) {
        perror("Ошибка создания семафора");
        exit(EXIT_FAILURE);
    }

    // Инициализация семафоров
    if (semctl(semid, 0, SETVAL, 1) == -1) { // Разрешение доступа к файлу
        perror("Ошибка инициализации семафора доступа");
        exit(EXIT_FAILURE);
    }
    if (semctl(semid, 1, SETVAL, 0) == -1) { // Семафор синхронизации читателей
        perror("Ошибка инициализации семафора синхронизации читателей");
        exit(EXIT_FAILURE);
    }
    if (semctl(semid, 2, SETVAL, 0) == -1) { // Семафор синхронизации записи
        perror("Ошибка инициализации семафора синхронизации записи");
        exit(EXIT_FAILURE);
    }

    int pipefd[NUM_READERS][2];
    pid_t pids[NUM_READERS];

    // Создание каналов и процессов
    for (int i = 0; i < NUM_READERS; i++) {
        if (pipe(pipefd[i]) == -1) {
            perror("Ошибка создания канала");
            exit(EXIT_FAILURE);
        }
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("Ошибка создания процесса");
            exit(EXIT_FAILURE);
        }
        if (pids[i] == 0) { // Дочерний процесс
            close(pipefd[i][0]);

            srand(time(NULL) ^ getpid());
            for (int j = 0; j < num_numbers; j++) {
                if (j > 0) {
                    wait_writer(); // Ждать завершения записи родителем
                }

                lock_semaphore();

                FILE* file = fopen("output.txt", "r");
                printf("\n[Читатель %d, Шаг %d] Читает файл:\n", i + 1, j + 1);
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

                unlock_semaphore();

                int num = rand() % 100;
                write(pipefd[i][1], &num, sizeof(num));

                signal_reader_done(); // Сигнал, что читатель завершил шаг
            }

            close(pipefd[i][1]);
            exit(EXIT_SUCCESS);
        }
    }

    // Родительский процесс
    for (int i = 0; i < NUM_READERS; i++) {
        close(pipefd[i][1]);
    }

    file = fopen("output.txt", "w");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        exit(EXIT_FAILURE);
    }

    int num;
    for (int i = 0; i < num_numbers; i++) {
        wait_all_readers(); // Ожидание завершения чтения всеми читателями

        for (int j = 0; j < NUM_READERS; j++) {
            if (read(pipefd[j][0], &num, sizeof(num)) <= 0) {
                perror("Ошибка чтения из канала");
                exit(EXIT_FAILURE);
            }

            lock_semaphore();

            printf("[Шаг %d, Читатель %d] Родитель получил число: %d\n\n", i + 1, j + 1, num);
            fprintf(file, "%d\n", num);
            fflush(file);

            unlock_semaphore();
        }

        // Сигнал всем читателям, что запись завершена
        for (int j = 0; j < NUM_READERS; j++) {
            signal_writer_done();
        }
    }

    fclose(file);
    for (int i = 0; i < NUM_READERS; i++) {
        close(pipefd[i][0]);
        wait(NULL);
    }

    // Удаление семафора
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("Ошибка удаления семафора");
    }

    // Вывод итогового содержимого файла
    print_final_file();

    return EXIT_SUCCESS;
}
