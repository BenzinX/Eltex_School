#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 512

struct message {
    long mtype;          // Приоритет сообщения
    char mtext[MAX_TEXT]; // Текст сообщения
};

int main() {
    key_t key = ftok("chat", 52); // Генерация ключа
    int msqid = msgget(key, 0666 | IPC_CREAT); // Подключение к очереди
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    struct message msg;
    char buffer[MAX_TEXT];

    // Chat2 начинает первым
    printf("Chat2: ");
    fgets(buffer, MAX_TEXT, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Удаление '\n'

    msg.mtype = 1; // Обычное сообщение
    strncpy(msg.mtext, buffer, MAX_TEXT);

    if (msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    if (strcmp(buffer, "exit") == 0) {
        msg.mtype = 2; // Сообщение с приоритетом завершения
        strncpy(msg.mtext, "exit", MAX_TEXT);
        msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
        msgctl(msqid, IPC_RMID, NULL);
        exit(0);
    }

    while (1) {
        // Получение сообщения с приоритетом 1
        if (msgrcv(msqid, &msg, sizeof(msg.mtext), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Chat1: %s\n", msg.mtext);

        // Проверка на завершение
        if (strcmp(msg.mtext, "exit") == 0) {
            msgctl(msqid, IPC_RMID, NULL);
            break;
        }

        // Отправка ответа
        printf("Chat2: ");
        fgets(buffer, MAX_TEXT, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        msg.mtype = 1;
        strncpy(msg.mtext, buffer, MAX_TEXT);

        if (msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        if (strcmp(buffer, "exit") == 0) {
            msg.mtype = 2;
            strncpy(msg.mtext, "exit", MAX_TEXT);
            msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
            msgctl(msqid, IPC_RMID, NULL);
            break;
        }
    }

    return 0;
}
