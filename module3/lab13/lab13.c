#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_NUMS 100
#define PERMISSIONS 0666
#define SHM_NAME "/myshm"

// Структура для разделяемой памяти
struct shared_data {
    int nums[MAX_NUMS];
    int count;
    int max;
    int min;
    int ready;
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
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, PERMISSIONS);
    if (fd == -1) {
        perror("Ошибка при создании разделяемой памяти");
        exit(1);
    }

    // Установка размера разделяемой памяти
    if (ftruncate(fd, sizeof(struct shared_data)) == -1) {
        perror("Ошибка при установке размера разделяемой памяти");
        exit(1);
    }

    // Маппинг разделяемой памяти
    struct shared_data *shm = mmap(NULL, sizeof(struct shared_data), 
                                  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("Ошибка при маппинге разделяемой памяти");
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
        munmap(shm, sizeof(struct shared_data));
        shm_unlink(SHM_NAME);
    }

    close(fd);
    return 0;
}

// Подпрограмма для дочернего процесса
void child_process(struct shared_data *shm) {
    while (running) {
        if (shm->ready) {
            shm->max = shm->nums[0];
            shm->min = shm->nums[0];
            for (int i = 1; i < shm->count; i++) {
                if (shm->nums[i] > shm->max) shm->max = shm->nums[i];
                if (shm->nums[i] < shm->min) shm->min = shm->nums[i];
            }
            shm->ready = 0;
        }
    }
    munmap(shm, sizeof(struct shared_data));
    exit(0);
}

// Подпрограмма для родительского процесса
void parent_process(struct shared_data *shm, pid_t pid) {
    while (running) {
        if (!shm->ready) {
            shm->count = rand() % MAX_NUMS + 1;
            for (int i = 0; i < shm->count; i++) {
                shm->nums[i] = (rand() % 500) + 1;
            }
            shm->ready = 1;
            processed_sets++;

            while (shm->ready) usleep(1000);

            if (running) {
                printf("Набор %d-й: MAX = %d, MIN = %d\n", 
                       processed_sets, shm->max, shm->min);
            }
        }
        usleep(100000);
    }

    kill(pid, SIGTERM);
    wait(NULL);
    printf("Обработано наборов: %d\n", processed_sets);
}
