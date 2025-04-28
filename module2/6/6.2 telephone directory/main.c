#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include "contact_list.h"

#define FILENAME "book.dat"

void saveToFile(ContactList* list);
void loadFromFile(ContactList* list);
void menuOutput();
void outList(ContactList* list);
void editField(Contact* contact);
void deleteField(Contact* contact);

int main() {
    setlocale(LC_ALL, "Russian");
    ContactList list;
    initList(&list);
    loadFromFile(&list);

    int _id;
    outList(&list);

    while (1) {
        menuOutput();
        int choice;
        scanf("%d", &choice);
        while (choice > 7 || choice < 1) {
            printf("Сделайте корректный выбор\n");
            scanf("%d", &choice);
        }
        switch (choice) {
        case 1: {
            Contact newContact = {0};
            printf("Введите имя и фамилию: ");
            scanf("%s %s", newContact.firstName, newContact.lastName);
            addContact(&list, &newContact);
            printf("Контакт %s %s добавлен\n", newContact.firstName, newContact.lastName);
            break;
        }
        case 2: {
            outList(&list);
            printf("Введите id контакта: ");
            scanf("%d", &_id);
            Contact newData = {0};
            printf("Что вы хотели бы изменить?\n1. Добавить или изменить поле\n2. Удалить поле\n");
            int subChoice;
            scanf("%d", &subChoice);
            if (subChoice == 1) editField(&newData);
            else if (subChoice == 2) deleteField(&newData);
            if (editContact(&list, _id, &newData) == -1) {
                printf("Контакт с ID %d не найден\n", _id);
            }
            break;
        }
        case 3: {
            outList(&list);
            printf("Введите id контакта: ");
            scanf("%d", &_id);
            if (deleteContact(&list, _id) == -1) {
                printf("Контакт с ID %d не найден\n", _id);
            }
            break;
        }
        case 4:
            outList(&list);
            break;
        case 5:
            saveToFile(&list);
            break;
        case 6:
            freeList(&list);
            return 0;
        }
    }
}

void menuOutput() {
    printf("1. Добавить контакт\n2. Изменить контакт\n3. Удалить контакт\n4. Вывод телефонной книги\n5. Сохранить телефонную книгу\n6. Выйти из программы\n");
}

void outList(ContactList* list) {
    if (isListEmpty(list)) {
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

void editField(Contact* contact) {
    printf("Какое поле вы хотели бы изменить?\n1. Имя\n2. Фамилия\n3. Место работы\n4. Должность\n5. Номер телефона\n6. Почтовый адрес\n7. Ссылка на соцсеть\n");
    int choice;
    scanf("%d", &choice);
    char input[MAX_URL_LEN];
    switch (choice) {
        case 1: printf("Введите новое имя: "); scanf("%s", input); strncpy(contact->firstName, input, MAX_NAME_LEN); break;
        case 2: printf("Введите новую фамилию: "); scanf("%s", input); strncpy(contact->lastName, input, MAX_NAME_LEN); break;
        case 3: printf("Введите место работы: "); scanf("%s", input); strncpy(contact->job, input, MAX_JOB_LEN); break;
        case 4: 
            if (strlen(contact->job) == 0) {
                printf("Введите место работы: "); scanf("%s", input); strncpy(contact->job, input, MAX_JOB_LEN);
            }
            printf("Введите должность: "); scanf("%s", input); strncpy(contact->position, input, MAX_JOB_LEN); break;
        case 5: printf("Введите номер телефона: "); scanf("%s", input); strncpy(contact->phoneNumber, input, MAX_PHONE_LEN); break;
        case 6: printf("Введите почтовый адрес: "); scanf("%s", input); strncpy(contact->email, input, MAX_EMAIL_LEN); break;
        case 7: printf("Введите ссылку на соцсеть: "); scanf("%s", input); strncpy(contact->url, input, MAX_URL_LEN); break;
    }
}

void deleteField(Contact* contact) {
    printf("Какое поле вы хотели бы удалить?\n1. Место работы\n2. Должность\n3. Номер телефона\n4. Почтовый адрес\n5. Ссылка на социальную сеть\n");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
        case 1: contact->job[0] = '\0'; break;
        case 2: contact->position[0] = '\0'; break;
        case 3: contact->phoneNumber[0] = '\0'; break;
        case 4: contact->email[0] = '\0'; break;
        case 5: contact->url[0] = '\0'; break;
    }
}

void saveToFile(ContactList* list) {
    FILE* file = fopen(FILENAME, "wb");
    if (!file) {
        printf("Ошибка открытия файла для записи\n");
        return;
    }

    fwrite(&list->contactCount, sizeof(int), 1, file);
    ContactNode* current = list->head;
    while (current != NULL) {
        fwrite(&current->data, sizeof(Contact), 1, file);
        current = current->next;
    }

    fclose(file);
    printf("Данные успешно сохранены в файл\n");
}

void loadFromFile(ContactList* list) {
    FILE* file = fopen(FILENAME, "rb");
    if (!file) {
        printf("Файл не найден, будет создан новый при сохранении\n");
        return;
    }

    int count;
    if (fread(&count, sizeof(int), 1, file) != 1) {
        printf("Ошибка чтения количества контактов\n");
        fclose(file);
        return;
    }

    for (int i = 0; i < count; i++) {
        ContactNode* newContact = (ContactNode*)malloc(sizeof(ContactNode));
        if (!newContact) {
            printf("Ошибка выделения памяти\n");
            break;
        }

        if (fread(&newContact->data, sizeof(Contact), 1, file) != 1) {
            printf("Ошибка чтения контакта\n");
            free(newContact);
            break;
        }

        newContact->prev = list->tail;
        newContact->next = NULL;

        if (list->tail != NULL) {
            list->tail->next = newContact;
        } else {
            list->head = newContact;
        }

        list->tail = newContact;
        list->contactCount++;
    }

    fclose(file);
}
