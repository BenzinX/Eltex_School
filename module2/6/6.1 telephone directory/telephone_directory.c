#include "telephone_directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Для совместимости с Linux
#ifdef __linux__
#define scanf_s(format, ptr, ...) scanf(format, ptr)
#define strncpy_s(dst, dst_size, src, count) strncpy(dst, src, count)
#define fopen_s(pFile, filename, mode) ((*(pFile)) = fopen((filename), (mode))) ? 0 : 1
#endif

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

void addContact(ContactList* list) {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    printf("Введите имя и фамилию: ");
    scanf("%s %s", firstName, lastName);

    // Создаем новый контакт
    ContactNode* newContact = (ContactNode*)malloc(sizeof(ContactNode));
    if (newContact == NULL) {
        printf("Ошибка выделения памяти\n");
        return;
    }

    // Заполняем обязательные поля
    newContact->data.id = list->contactCount + 1;
    strncpy_s(newContact->data.firstName, MAX_NAME_LEN, firstName, MAX_NAME_LEN);
    strncpy_s(newContact->data.lastName, MAX_NAME_LEN, lastName, MAX_NAME_LEN);

    // Инициализируем остальные поля пустыми строками
    strncpy_s(newContact->data.job, MAX_JOB_LEN, "", MAX_JOB_LEN);
    strncpy_s(newContact->data.position, MAX_JOB_LEN, "", MAX_JOB_LEN);
    strncpy_s(newContact->data.phoneNumber, MAX_PHONE_LEN, "", MAX_PHONE_LEN);
    strncpy_s(newContact->data.email, MAX_EMAIL_LEN, "", MAX_EMAIL_LEN);
    strncpy_s(newContact->data.url, MAX_URL_LEN, "", MAX_URL_LEN);

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

    printf("Контакт %s %s добавлен\n", newContact->data.firstName, newContact->data.lastName);
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

    printf("Что вы хотели бы изменить?\n");
    printf("1. Добавить или изменить поле\n");
    printf("2. Удалить поле\n");
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
    printf("Какое поле вы хотели бы изменить?\n");
    printf("1. Имя\n");
    printf("2. Фамилия\n");
    printf("3. Место работы\n");
    printf("4. Должность\n");
    printf("5. Номер телефона\n");
    printf("6. Почтовый адрес\n");
    printf("7. Ссылка на социальную сеть\n");
    int i;
    scanf_s("%d", &i);

    char input[MAX_URL_LEN]; // Максимальный размер среди всех полей

    switch (i) {
    case 1:
        printf("Введите новое имя: ");
        scanf_s("%s", input, MAX_NAME_LEN);
        strncpy_s(contact->firstName, MAX_NAME_LEN, input, MAX_NAME_LEN);
        break;
    case 2:
        printf("Введите новую фамилию: ");
        scanf_s("%s", input, MAX_NAME_LEN);
        strncpy_s(contact->lastName, MAX_NAME_LEN, input, MAX_NAME_LEN);
        break;
    case 3:
        printf("Введите место работы: ");
        scanf_s("%s", input, MAX_JOB_LEN);
        strncpy_s(contact->job, MAX_JOB_LEN, input, MAX_JOB_LEN);
        break;
    case 4:
        if (strlen(contact->job) == 0) {
            printf("Введите место работы: ");
            scanf_s("%s", input, MAX_JOB_LEN);
            strncpy_s(contact->job, MAX_JOB_LEN, input, MAX_JOB_LEN);
        }
        printf("Введите должность: ");
        scanf_s("%s", input, MAX_JOB_LEN);
        strncpy_s(contact->position, MAX_JOB_LEN, input, MAX_JOB_LEN);
        break;
    case 5:
        printf("Введите номер телефона: ");
        scanf_s("%s", input, MAX_PHONE_LEN);
        strncpy_s(contact->phoneNumber, MAX_PHONE_LEN, input, MAX_PHONE_LEN);
        break;
    case 6:
        printf("Введите почтовый адрес: ");
        scanf_s("%s", input, MAX_EMAIL_LEN);
        strncpy_s(contact->email, MAX_EMAIL_LEN, input, MAX_EMAIL_LEN);
        break;
    case 7:
        printf("Введите ссылку на соцсеть: ");
        scanf_s("%s", input, MAX_URL_LEN);
        strncpy_s(contact->url, MAX_URL_LEN, input, MAX_URL_LEN);
        break;
    default:
        break;
    }
}

void deleteField(Contact* contact) {
    printf("Какое поле вы хотели бы удалить?\n");
    printf("1. Место работы\n");
    printf("2. Должность\n");
    printf("3. Номер телефона\n");
    printf("4. Почтовый адрес\n");
    printf("5. Ссылка на социальную сеть\n");
    int i;
    scanf_s("%d", &i);

    switch (i) {
    case 1:
        strncpy_s(contact->job, MAX_JOB_LEN, "", MAX_JOB_LEN);
        break;
    case 2:
        strncpy_s(contact->position, MAX_JOB_LEN, "", MAX_JOB_LEN);
        break;
    case 3:
        strncpy_s(contact->phoneNumber, MAX_PHONE_LEN, "", MAX_PHONE_LEN);
        break;
    case 4:
        strncpy_s(contact->email, MAX_EMAIL_LEN, "", MAX_EMAIL_LEN);
        break;
    case 5:
        strncpy_s(contact->url, MAX_URL_LEN, "", MAX_URL_LEN);
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
