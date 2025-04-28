#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void calculate_areas(int start, int end, char *argv[]) {
    for (int i = start; i < end; i++) {
        double side = atof(argv[i]);
        double area = side * side;
        printf("Квадрат %d (сторона %.2f): площадь = %.2f\n", 
               i, side, area);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s сторона1 [сторона2 ...]\n", argv[0]);
        return 1;
    }

    int count = argc - 1;
    int half = count / 2;

    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка при создании процесса");
        return 1;
    } else if (pid == 0) {
        // Дочерний процесс ждёт сигнала от родителя
        printf("Дочерний процесс (PID: %d)\n", getpid());
        calculate_areas(half + 1, argc, argv);
    } else {
        // Родительский процесс сначала выполняет свою часть
        printf("Родительский процесс (PID: %d)\n", getpid());
        calculate_areas(1, half + 1, argv);
        
        // Ждём завершения дочернего процесса
        wait(NULL);  
    }

    return 0;
}
