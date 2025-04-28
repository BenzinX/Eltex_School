#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define QUEUE_NAME "/chat_queue"
#define MAX_TEXT 512

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_TEXT];
    unsigned int prio;

    // Настройка атрибутов очереди
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_TEXT;
    attr.mq_curmsgs = 0;

    // Подключение к очереди
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    // Chat2 начинает первым
    printf("Chat2: ");
    fgets(buffer, MAX_TEXT, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    prio = 1; // Обычное сообщение
    if (mq_send(mq, buffer, strlen(buffer) + 1, prio) == -1) {
        perror("mq_send");
        exit(1);
    }

    if (strcmp(buffer, "exit") == 0) {
        prio = 2; // Приоритет завершения
        mq_send(mq, buffer, strlen(buffer) + 1, prio);
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        exit(0);
    }

    while (1) {
        // Получение сообщения
        if (mq_receive(mq, buffer, MAX_TEXT, &prio) == -1) {
            perror("mq_receive");
            exit(1);
        }

        buffer[strcspn(buffer, "\0")] = 0;
        printf("Chat1: %s\n", buffer);

        // Проверка на завершение
        if (prio == 2 && strcmp(buffer, "exit") == 0) {
            mq_close(mq);
            mq_unlink(QUEUE_NAME);
            break;
        }

        // Отправка ответа
        printf("Chat2: ");
        fgets(buffer, MAX_TEXT, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        prio = 1;
        if (mq_send(mq, buffer, strlen(buffer) + 1, prio) == -1) {
            perror("mq_send");
            exit(1);
        }

        if (strcmp(buffer, "exit") == 0) {
            prio = 2;
            mq_send(mq, buffer, strlen(buffer) + 1, prio);
            mq_close(mq);
            mq_unlink(QUEUE_NAME);
            break;
        }
    }

    return 0;
}
