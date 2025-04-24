#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

// Структура элемента очереди
typedef struct Node {
    int data;          // Данные элемента
    int priority;      // Приоритет элемента
    struct Node* next; // Указатель на следующий элемент
} Node;

// Структура очереди
typedef struct {
    Node* front; // Указатель на начало очереди
    int count; // Количество элементов очереди
} PriorityQueue;

// Инициализация очереди
void initQueue(PriorityQueue* q) {
    q->count = 0;
    q->front = NULL;
}

// 1) Добавление элемента в очередь
void push(PriorityQueue* q, int data, int priority) {

    Node* newNode = malloc(sizeof(Node));

    newNode->data = data;
    newNode->priority = priority;
    newNode->next = NULL;

    // Если очередь пуста
    if (q->front == NULL) {
        q->front = newNode;
        return;
    }

    Node* current = q->front;
    Node* prev = NULL;

    // Ищем место для вставки
    while (current != NULL && current->priority <= priority) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        // Вставка в начало (наш элемент имеет наивысший приоритет)
        newNode->next = q->front;
        q->front = newNode;
    }
    else {
        // Вставка в середину
        prev->next = newNode;
        newNode->next = current;
    }
}

// 2) Извлечение элемента из начала очереди (pop)
int pop(PriorityQueue* q, int* data, int* priority) {
    if (q->front == NULL) {
        return 0; // Очередь пуста
    }

    Node* temp = q->front;
    *data = temp->data;
    *priority = temp->priority;

    q->front = q->front->next;

    free(temp);
    return 1;
}

// 3) Извлечение элемента с указанным приоритетом
int popWithPriority(PriorityQueue* q, int targetPriority, int* data) {
    if (q->front == NULL) {
        return 0; // Очередь пуста
    }

    Node* current = q->front, * prev = NULL;

    // Поиск элемента с нужным приоритетом
    while (current != NULL && current->priority != targetPriority) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Элемент с заданным приоритетом не найден\n");
        return 0; // Элемент с таким приоритетом не найден
    }

    *data = current->data;

    // Удаление элемента из очереди
    if (prev == NULL) {
        // Удаляем первый элемент
        q->front = current->next;
    }
    else {
        prev->next = current->next;
    }

    free(current);
    return 1;
}

// 4) Извлечение элемента с приоритетом не ниже заданного
int popWithMinPriority(PriorityQueue* q, int minPriority, int* data, int* foundElementPriority) {
    if (q->front == NULL) {
        return 0; // Очередь пуста
    }

    Node* current = q->front, * prev = NULL;

    // Поиск элемента с нужным приоритетом
    while (current != NULL && current->priority > minPriority) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Элемент с заданным приоритетом не найден\n");
        return 0; // Элемент с таким приоритетом не найден
    }

    *data = current->data;
    *foundElementPriority = current->priority;

    // Удаление элемента из очереди
    if (prev == NULL) {
        // Удаляем первый элемент
        q->front = current->next;
    }
    else {
        prev->next = current->next;
    }

    free(current);
    return 1;
}

// Функция для печати очереди
void printQueue(const PriorityQueue* q) {
    const Node* current = q->front;
    printf("Очередь:\n");
    while (current != NULL) {
        printf("(№ %d, приоритет %d)\n", current->data, current->priority);
        current = current->next;
    }
    printf("\n");
}

void fillqueue(PriorityQueue* q, int N) {
    for (int i = 1; i <= N; i++, q->count++) {
        push(q, (q->count + 1) * 10, rand() % 256);
    }
}

int main() {
    setlocale(LC_ALL, "Russian"); // Устанавливаем русскую локаль

    srand(time(NULL));

    PriorityQueue q;
    initQueue(&q);

    while (1) {
        printf("1. Заполнить очередь N элементами\n");
        printf("2. Извлечь первый элемент из очереди\n");
        printf("3. Извлечь элемент с заданным приоритетом\n");
        printf("4. Извлечь элемент с приоритетом не ниже заданного\n");
        printf("5. Вывести очередь\n");
        printf("6. Выйти из программы\n> ");
        int N, i, data, priority;
        scanf_s("%d", &i);

        switch (i)
        {
        case 1:
            printf("Сколько сообщений сгенерировать? ");
            scanf_s("%d", &N);
            // Добавляем элементы и выводим очередь
            fillqueue(&q, N);
            printQueue(&q);
            break;
        case 2:
            if (pop(&q, &data, &priority)) {
                printf("Извлечен из начала:\n %d (приоритет %d)\n", data, priority);
            }
            break;
        case 3:
            printf("С каким приоритетом извлечь элемент? \n");
            scanf_s("%d", &priority);

            // Извлекаем с конкретным приоритетом
            if (popWithPriority(&q, priority, &data)) {
                printf("Извлечен с приоритетом %d: сообщение №%d\n", priority, data);
            }
            break;
        case 4:
            // Извлекаем с минимальным приоритетом
            printf("С каким минимальным приоритетом извлечь элемент? \n");
            scanf_s("%d", &priority);
            int foundElementPriotity;

            if (popWithMinPriority(&q, priority, &data, &foundElementPriotity)) {
                printf("Извлечен элемент с приоритетом не ниже %d: сообщение № %d с приоритетом %d\n", priority, data, foundElementPriotity);
            }
            break;
        case 5:
            printQueue(&q);
            break;
        case 6:
            return 1;
        default:
            break;
        }
    }
    return 0;
}
