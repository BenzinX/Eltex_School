#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

// Максимальные длины для строковых полей
#define MAX_NAME_LEN 30
#define MAX_JOB_LEN 30
#define MAX_PHONE_LEN 20
#define MAX_EMAIL_LEN 50
#define MAX_URL_LEN 100
#define FILENAME "book.dat"

typedef struct {
    int id;
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char job[MAX_JOB_LEN];
    char position[MAX_JOB_LEN];
    char phoneNumber[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
    char url[MAX_URL_LEN];
} Contact;

// Узел списка
typedef struct ContactNode {
    Contact data;
    struct ContactNode* prev;
    struct ContactNode* next;
} ContactNode;

// Список
typedef struct {
    ContactNode* head;
    ContactNode* tail;
    int contactCount;
} ContactList;

// Прототипы функций
void saveToFile(ContactList* list);
void loadFromFile(ContactList* list);
void addContact(ContactList* list);
int editContact(ContactList* list, int id);
int deleteContact(ContactList* list, int id);
void editField(Contact* contact);
void deleteField(Contact* contact);
int isListEmpty(ContactList* list);
void outList(ContactList* list);
void freeList(ContactList* list);
void menuOutput();

int main() {
    setlocale(LC_ALL, "Russian");
    ContactList list = { NULL, NULL, 0 };
    loadFromFile(&list);

    int _id;
    outList(&list);

    while (1) {
        menuOutput();
        int i;
        scanf_s("%d", &i);
        while (i > 7 || i < 1) {
            printf_s("Сделайте корректный выбор\n");
            scanf_s("%d", &i);
        }
        switch (i) {
        case 1:
            addContact(&list);
            break;
        case 2:
            outList(&list);
            printf_s("Введите id контакта: ");
            scanf_s("%d", &_id);
            editContact(&list, _id);
            break;
        case 3:
            outList(&list);
            printf_s("Введите id контакта: ");
            scanf_s("%d", &_id);
            deleteContact(&list, _id);
            break;
        case 4:
            outList(&list);
            break;
        case 5:
            saveToFile(&list);
            break;
        case 6:
            freeList(&list);
            return 0;
        default:
            break;
        }
    }
}

void menuOutput() {
    printf_s("1. Добавить контакт\n");
    printf_s("2. Изменить контакт\n");
    printf_s("3. Удалить контакт\n");
    printf_s("4. Вывод телефонной книги\n");
    printf_s("5. Сохранить телефонную книгу\n");
    printf_s("6. Выйти из программы\n");
}

void outList(ContactList* list) {
    if (list == NULL || list->head == NULL) {
        printf("Телефонная книга пуста\n");
        return;
    }

    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| ID | Имя         | Фамилия     | Работа                  | Должность              | Телефон          | Email                                | Соцсеть                                    |\n");
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    ContactNode* current = list->head;
    while (current != NULL) {
        printf("| %2d | %-11s | %-12s | %-22s | %-22s | %-16s | %-36s | %-42s |\n",
            current->data.id, current->data.firstName, current->data.lastName,
            current->data.job, current->data.position, current->data.phoneNumber,
            current->data.email, current->data.url);
        current = current->next;
    }
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

void addContact(ContactList* list) {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    printf_s("Введите имя и фамилию: ");
    scanf_s("%s %s", firstName, (unsigned)_countof(firstName), lastName, (unsigned)_countof(lastName));

    // Создаем новый контакт
    ContactNode* newContact = (ContactNode*)malloc(sizeof(ContactNode));
    if (newContact == NULL) {
        printf("Ошибка выделения памяти\n");
        return;
    }

    // Заполняем обязательные поля
    newContact->data.id = list->contactCount + 1;
    strncpy_s(newContact->data.firstName, sizeof(newContact->data.firstName), firstName, _TRUNCATE);
    strncpy_s(newContact->data.lastName, sizeof(newContact->data.lastName), lastName, _TRUNCATE);

    // Инициализируем остальные поля пустыми строками
    strncpy_s(newContact->data.job, sizeof(newContact->data.job), "", _TRUNCATE);
    strncpy_s(newContact->data.position, sizeof(newContact->data.position), "", _TRUNCATE);
    strncpy_s(newContact->data.phoneNumber, sizeof(newContact->data.phoneNumber), "", _TRUNCATE);
    strncpy_s(newContact->data.email, sizeof(newContact->data.email), "", _TRUNCATE);
    strncpy_s(newContact->data.url, sizeof(newContact->data.url), "", _TRUNCATE);

    // Добавляем в список (в конец)
    newContact->prev = list->tail;
    newContact->next = NULL;

    if (list->tail != NULL) {
        list->tail->next = newContact;
    }
    else {
        // Если список был пуст
        list->head = newContact;
    }

    list->tail = newContact;
    list->contactCount++;

    printf_s("Контакт %s %s добавлен\n", newContact->data.firstName, newContact->data.lastName);
}

int editContact(ContactList* list, int id) {
    if (isListEmpty(list)) return -1;

    // Находим контакт по ID
    ContactNode* current = list->head;
    while (current != NULL && current->data.id != id) {
        current = current->next;
    }

    if (current == NULL) {
        printf("Контакт с ID %d не найден\n", id);
        return -1;
    }

    printf_s("Что вы хотели бы изменить?\n");
    printf_s("1. Добавить или изменить поле\n");
    printf_s("2. Удалить поле\n");
    int i;
    scanf_s("%d", &i);
    switch (i) {
    case 1:
        editField(&current->data);
        break;
    case 2:
        deleteField(&current->data);
        break;
    default:
        break;
    }

    return 1;
}

void editField(Contact* contact) {
    printf_s("Какое поле вы хотели бы изменить?\n");
    printf_s("1. Имя\n");
    printf_s("2. Фамилия\n");
    printf_s("3. Место работы\n");
    printf_s("4. Должность\n");
    printf_s("5. Номер телефона\n");
    printf_s("6. Почтовый адрес\n");
    printf_s("7. Ссылка на социальную сеть\n");
    int i;
    scanf_s("%d", &i);

    char input[MAX_URL_LEN]; // Максимальный размер среди всех полей

    switch (i) {
    case 1:
        printf_s("Введите новое имя: ");
        scanf_s("%s", input, (unsigned)_countof(input));
        strncpy_s(contact->firstName, sizeof(contact->firstName), input, _TRUNCATE);
        break;
    case 2:
        printf_s("Введите новую фамилию: ");
        scanf_s("%s", input, (unsigned)_countof(input));
        strncpy_s(contact->lastName, sizeof(contact->lastName), input, _TRUNCATE);
        break;
    case 3:
        printf_s("Введите место работы: ");
        scanf_s("%s", input, (unsigned)_countof(input));
        strncpy_s(contact->job, sizeof(contact->job), input, _TRUNCATE);
        break;
    case 4:
        if (strlen(contact->job) == 0) {
            printf_s("Введите место работы: ");
            scanf_s("%s", input, (unsigned)_countof(input));
            strncpy_s(contact->job, sizeof(contact->job), input, _TRUNCATE);
        }
        printf_s("Введите должность: ");
        scanf_s("%s", input, (unsigned)_countof(input));
        strncpy_s(contact->position, sizeof(contact->position), input, _TRUNCATE);
        break;
    case 5:
        printf_s("Введите номер телефона: ");
        scanf_s("%s", input, (unsigned)_countof(input));
        strncpy_s(contact->phoneNumber, sizeof(contact->phoneNumber), input, _TRUNCATE);
        break;
    case 6:
        printf_s("Введите почтовый адрес: ");
        scanf_s("%s", input, (unsigned)_countof(input));
        strncpy_s(contact->email, sizeof(contact->email), input, _TRUNCATE);
        break;
    case 7:
        printf_s("Введите ссылку на соцсеть: ");
        scanf_s("%s", input, (unsigned)_countof(input));
        strncpy_s(contact->url, sizeof(contact->url), input, _TRUNCATE);
        break;
    default:
        break;
    }
}

void deleteField(Contact* contact) {
    printf_s("Какое поле вы хотели бы удалить?\n");
    printf_s("1. Место работы\n");
    printf_s("2. Должность\n");
    printf_s("3. Номер телефона\n");
    printf_s("4. Почтовый адрес\n");
    printf_s("5. Ссылка на социальную сеть\n");
    int i;
    scanf_s("%d", &i);

    switch (i) {
    case 1:
        strncpy_s(contact->job, sizeof(contact->job), "", _TRUNCATE);
        break;
    case 2:
        strncpy_s(contact->position, sizeof(contact->position), "", _TRUNCATE);
        break;
    case 3:
        strncpy_s(contact->phoneNumber, sizeof(contact->phoneNumber), "", _TRUNCATE);
        break;
    case 4:
        strncpy_s(contact->email, sizeof(contact->email), "", _TRUNCATE);
        break;
    case 5:
        strncpy_s(contact->url, sizeof(contact->url), "", _TRUNCATE);
        break;
    default:
        break;
    }
}

int isListEmpty(ContactList* list) {
    return (list == NULL || list->head == NULL);
}

int deleteContact(ContactList* list, int id) {
    if (isListEmpty(list)) return -1;

    // Находим контакт по ID
    ContactNode* current = list->head;
    while (current != NULL && current->data.id != id) {
        current = current->next;
    }

    if (current == NULL) {
        printf("Контакт с ID %d не найден\n", id);
        return -1;
    }

    // Обновляем связи в списке
    if (current->prev != NULL) {
        current->prev->next = current->next;
    }
    else {
        // Удаляем голову списка
        list->head = current->next;
    }

    if (current->next != NULL) {
        current->next->prev = current->prev;
    }
    else {
        // Удаляем хвост списка
        list->tail = current->prev;
    }

    // Обновляем ID оставшихся контактов
    ContactNode* temp = current->next;
    while (temp != NULL) {
        temp->data.id--;
        temp = temp->next;
    }

    free(current);
    list->contactCount--;

    return 1;
}

void saveToFile(ContactList* list) {
    FILE* file;
    if (fopen_s(&file, FILENAME, "wb") != 0) {
        printf("Ошибка открытия файла для записи\n");
        return;
    }

    // Сначала записываем количество контактов
    fwrite(&list->contactCount, sizeof(int), 1, file);

    // Затем записываем каждый контакт
    ContactNode* current = list->head;
    while (current != NULL) {
        fwrite(&current->data, sizeof(Contact), 1, file);
        current = current->next;
    }

    fclose(file);
    printf("Данные успешно сохранены в файл\n");
}

void loadFromFile(ContactList* list) {
    FILE* file;
    if (fopen_s(&file, FILENAME, "rb") != 0) {
        printf("Файл не найден, будет создан новый при сохранении\n");
        return;
    }

    // Считываем количество контактов
    int count;
    if (fread(&count, sizeof(int), 1, file) != 1) {
        printf("Ошибка чтения количества контактов\n");
        fclose(file);
        return;
    }
    ContactNode* newContact;

    // Считываем каждый контакт и добавляем в список
    for (int i = 0; i < count; i++) {
        // Выделяем память под новый узел
        newContact = (ContactNode*)malloc(sizeof(ContactNode));
        if (newContact == NULL) {
            printf("Ошибка выделения памяти\n");
            break;
        }

        // Читаем данные контакта в data-часть узла
        if (fread(&newContact->data, sizeof(Contact), 1, file) != 1) {
            printf("Ошибка чтения контакта\n");
            free(newContact);
            break;
        }

        // Инициализируем указатели узла
        newContact->prev = list->tail;
        newContact->next = NULL;

        // Добавляем узел в список
        if (list->tail != NULL) {
            list->tail->next = newContact;
        }
        else {
            list->head = newContact;
        }

        list->tail = newContact;
        list->contactCount++;
    }

    fclose(file);
}

// Деструктор списка
void freeList(ContactList* list) {
    ContactNode* current = list->head;
    while (current != NULL) {
        ContactNode* next = current->next;
        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->contactCount = 0;
}
