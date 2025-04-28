#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define MAX_NUMS 100
#define PERMISSIONS 0666

// Структура для разделяемой памяти
struct shared_data {
    int nums[MAX_NUMS]; // Массив чисел
    int count;          // Количество чисел в наборе
    int max;            // Максимум
    int min;            // Минимум
    int ready;          // Флаг готовности данных
};

volatile sig_atomic_t running = 1;
int processed_sets = 0;

// Обработчик SIGINT
void sigint_handler(int sig) {
    running = 0;
}

void child_process(struct shared_data *shm);
void parent_process(struct shared_data *shm, pid_t pid);

int main() {
    srand(time(NULL));
    signal(SIGINT, sigint_handler);

    // Создание разделяемой памяти
    int shmid = shmget(IPC_PRIVATE, sizeof(struct shared_data), IPC_CREAT | PERMISSIONS);
    if (shmid == -1) {
        perror("Ошибка при инициализации разделяемой памяти");
        exit(1);
    }

    // Подключение разделяемой памяти
    struct shared_data *shm = (struct shared_data *)shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("Ошибка при подключении разделяемой памяти");
        exit(1);
    }
    shm->ready = 0;

    pid_t pid = fork();
    if (pid == -1) {
        perror("Ошибка при создании дочернего процесса");
        exit(1);
    }

    if (pid == 0) { // Дочерний процесс
        child_process(shm);
    } else { // Родительский процесс
        parent_process(shm, pid);
        shmdt(shm); // Отсоединяем сегмент разделяемой памяти
        shmctl(shmid, IPC_RMID, NULL); // Удаляем сегмент разделяемой памяти
    }

    return 0;
}

// Подпрограмма для дочернего процесса
void child_process(struct shared_data *shm) {
    while (running) {
        if (shm->ready) { // Проверка доступности разделяемой памяти
            // Поиск минимума и максимума
            shm->max = shm->nums[0]; // Задаем как максимум/минимум первым элементом
            shm->min = shm->nums[0];
            for (int i = 1; i < shm->count; i++) { // Перебор последовательности
                if (shm->nums[i] > shm->max) shm->max = shm->nums[i]; // Проверяем на максимум
                if (shm->nums[i] < shm->min) shm->min = shm->nums[i]; // Проверяем на минимум
            }
            shm->ready = 0; // Сигнал родителю о завершении операций
        }
    }
    shmdt(shm); // Отсоединяем разделяемую память
    exit(0);
}

// Подпрограмма для родительского процесса
void parent_process(struct shared_data *shm, pid_t pid) {
    while (running) {
        if (!shm->ready) {
            // Генерация набора чисел
            shm->count = rand() % MAX_NUMS + 1; // Случайное количество чисел
            for (int i = 0; i < shm->count; i++) {
                shm->nums[i] = (rand() % 500) + 1; // Случайные числа от 0 до 500
            }
            shm->ready = 1; // Сигнал дочернему процессу
            processed_sets++;

            // Ожидание обработки дочерним процессом
            while (shm->ready) usleep(1000);

            if (running) {
                printf("Набор %d-й: MAX = %d, MIN = %d\n", processed_sets, shm->max, shm->min);
            }
        }
        usleep(100000); //Для читаемости вывода
    }

    // Завершение
    kill(pid, SIGTERM);
    wait(NULL); // Ожидание завершения дочернего процесса
    printf("Обработано наборов: %d\n", processed_sets);
}
