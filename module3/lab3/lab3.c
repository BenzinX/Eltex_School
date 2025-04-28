#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// Максимальные длины для строковых полей
#define TABLE_SIZE 512
#define MAX_NAME_LEN 30
#define MAX_JOB_LEN 30
#define MAX_PHONE_LEN 20
#define MAX_EMAIL_LEN 50
#define MAX_URL_LEN 100
#define MAX_SOCIAL_LEN 100
#define FILENAME "../book.dat"

// Структура контакта
typedef struct {
    int id; // Уникальный идентификатор контакта
    char firstName[MAX_NAME_LEN]; // Имя (обязательное поле)
    char lastName[MAX_NAME_LEN]; // Фамилия (обязательное поле)
    char job[MAX_JOB_LEN]; // Место работы
    char position[MAX_JOB_LEN]; // Должность
    char phoneNumber[MAX_PHONE_LEN]; // Номер телефона
    char email[MAX_EMAIL_LEN]; // Адрес электронной почты
    char url[MAX_SOCIAL_LEN]; // Ссылка на соцсеть
} Contact;

// Таблица контактов
typedef struct {
    Contact contacts[TABLE_SIZE]; // Массив контактов
    int contactCount; // Количество контактов
} ContactTable;

void saveToFile(ContactTable* table);
void loadFromFile(ContactTable* table);
void addContact(ContactTable* table);
int editContact(ContactTable* table, int id);
int deleteContact(ContactTable* table, int id);
void editField(ContactTable* table, int id);
void deleteField(ContactTable* table, int id);
int isTableEmpty(ContactTable* table);
void outTable(ContactTable* table);
void menuOutput();

int main() {
    setlocale(LC_ALL, "Russian");
    ContactTable Table;
    Table.contactCount = 0;
    loadFromFile(&Table);
    int _id;
    outTable(&Table);

    while (1) {
        menuOutput();
        int i;
        scanf("%d", &i);
        while (i > 7 || i < 1) {
            printf("Сделайте корректный выбор\n");
            scanf("%d", &i);
        }
        switch (i) {
        case 1:
            addContact(&Table);
            break;
        case 2:
            outTable(&Table);
            printf("Введите id контакта: ");
            scanf("%d", &_id);
            editContact(&Table, _id);
            break;
        case 3:
            outTable(&Table);
            printf("Введите id контакта: ");
            scanf("%d", &_id);
            deleteContact(&Table, _id);
            break;
        case 4:
            outTable(&Table);
            break;
        case 5:
            saveToFile(&Table);
            break;
        case 6:
            return 0;
        default:
            break;
        }
    }
}

void menuOutput() {
    printf("1. Добавить контакт\n");
    printf("2. Изменить контакт\n");
    printf("3. Удалить контакт\n");
    printf("4. Вывод телефонной книги\n");
    printf("5. Сохранить телефонную книгу\n");
    printf("6. Выйти из программы\n");
}

void outTable(ContactTable* table) {
    if (table == NULL) {
        return;
    }

    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| ID | Имя         | Фамилия     | Работа                  | Должность              | Телефон          | Email                                | Соцсеть                                    |\n");
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < table->contactCount; i++) {
        Contact* contact = &(table->contacts[i]);
        printf("| %2d | %-11s | %-12s | %-22s | %-22s | %-16s | %-36s | %-42s |\n",
            contact->id, contact->firstName, contact->lastName,
            contact->job, contact->position, contact->phoneNumber,
            contact->email, contact->url);
    }
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

void addContact(ContactTable* table) {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    printf("Введите имя и фамилию: ");
    scanf("%s %s", firstName, lastName);

    Contact contact;
    contact.id = table->contactCount + 1;

    strncpy(contact.job, "", MAX_JOB_LEN);
    strncpy(contact.position, "", MAX_JOB_LEN);
    strncpy(contact.phoneNumber, "", MAX_PHONE_LEN);
    strncpy(contact.email, "", MAX_EMAIL_LEN);
    strncpy(contact.url, "", MAX_SOCIAL_LEN);
    strncpy(contact.firstName, firstName, MAX_NAME_LEN);
    strncpy(contact.lastName, lastName, MAX_NAME_LEN);

    table->contacts[table->contactCount] = contact;
    table->contactCount++;
    printf("Контакт %s %s добавлен\n", contact.firstName, contact.lastName);
}

int editContact(ContactTable* table, int id) {
    if (isTableEmpty(table)) return -1;

    printf("Что вы хотели бы изменить?\n");
    printf("1. Добавить или изменить поле\n");
    printf("2. Удалить поле\n");
    int i;
    scanf("%d", &i);

    switch (i) {
    case 1:
        editField(table, id);
        break;
    case 2:
        deleteField(table, id);
        break;
    default:
        break;
    }
    return 1;
}

void editField(ContactTable* table, int id) {
    printf("Какое поле вы хотели бы изменить?\n");
    printf("1. Имя\n");
    printf("2. Фамилия\n");
    printf("3. Место работы\n");
    printf("4. Должность\n");
    printf("5. Номер телефона\n");
    printf("6. Почтовый адрес\n");
    printf("7. Ссылка на социальную сеть\n");

    int i;
    scanf("%d", &i);

    switch (i) {
    case 1: {
        printf("Введите новое имя: ");
        char firstName[MAX_NAME_LEN];
        scanf("%s", firstName);
        strncpy(table->contacts[id - 1].firstName, firstName, MAX_NAME_LEN);
        break;
    }
    case 2: {
        printf("Введите новую фамилию: ");
        char lastName[MAX_NAME_LEN];
        scanf("%s", lastName);
        strncpy(table->contacts[id - 1].lastName, lastName, MAX_NAME_LEN);
        break;
    }
    case 3: {
        printf("Введите место работы: ");
        char job[MAX_JOB_LEN];
        scanf("%s", job);
        strncpy(table->contacts[id - 1].job, job, MAX_JOB_LEN);

        printf("Введите должность: ");
        char position[MAX_JOB_LEN];
        scanf("%s", position);
        strncpy(table->contacts[id - 1].position, position, MAX_JOB_LEN);
        break;
    }
    case 4: {
        printf("Введите должность: ");
        char position[MAX_JOB_LEN];
        scanf("%s", position);
        strncpy(table->contacts[id - 1].position, position, MAX_JOB_LEN);
        break;
    }
    case 5: {
        printf("Введите номер телефона: ");
        char phoneNumber[MAX_PHONE_LEN];
        scanf("%s", phoneNumber);
        strncpy(table->contacts[id - 1].phoneNumber, phoneNumber, MAX_PHONE_LEN);
        break;
    }
    case 6: {
        printf("Введите почтовый адрес: ");
        char email[MAX_EMAIL_LEN];
        scanf("%s", email);
        strncpy(table->contacts[id - 1].email, email, MAX_EMAIL_LEN);
        break;
    }
    case 7: {
        printf("Введите ссылку на соцсеть: ");
        char url[MAX_SOCIAL_LEN];
        scanf("%s", url);
        strncpy(table->contacts[id - 1].url, url, MAX_SOCIAL_LEN);
        break;
    }
    default:
        break;
    }
}

void deleteField(ContactTable* table, int id) {
    printf("Какое поле вы хотели бы удалить?\n");
    printf("1. Место работы\n");
    printf("2. Номер телефона\n");
    printf("3. Почтовый адрес\n");
    printf("4. Ссылка на социальную сеть\n");

    int i;
    scanf("%d", &i);

    switch (i) {
    case 1:
        strncpy(table->contacts[id - 1].job, "", MAX_JOB_LEN);
        strncpy(table->contacts[id - 1].position, "", MAX_JOB_LEN);
        break;
    case 2:
        strncpy(table->contacts[id - 1].phoneNumber, "", MAX_PHONE_LEN);
        break;
    case 3:
        strncpy(table->contacts[id - 1].email, "", MAX_EMAIL_LEN);
        break;
    case 4:
        strncpy(table->contacts[id - 1].url, "", MAX_SOCIAL_LEN);
        break;
    default:
        break;
    }
}

int isTableEmpty(ContactTable* table) {
    return (table == NULL || table->contactCount == 0);
}

int deleteContact(ContactTable* table, int id) {
    if (isTableEmpty(table)) return -1;

    int index = -1;
    for (int i = 0; i < table->contactCount; i++) {
        if (table->contacts[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return -1; // Контакт не найден
    }

    // Смещаем оставшиеся контакты
    for (int i = index; i < table->contactCount - 1; i++) {
        table->contacts[i] = table->contacts[i + 1];
        table->contacts[i].id = i + 1; // Обновляем ID
    }

    table->contactCount--;
    return 1;
}

void saveToFile(ContactTable* table) {
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        printf("Ошибка открытия файла для записи: %s\n", strerror(errno));
        return;
    }

    if (write(fd, &table->contactCount, sizeof(int)) == -1) {
        printf("Ошибка записи в файл: %s\n", strerror(errno));
        close(fd);
        return;
    }

    for (int i = 0; i < table->contactCount; i++) {
        if (write(fd, &table->contacts[i], sizeof(Contact)) == -1) {
            printf("Ошибка записи контакта %d: %s\n", i, strerror(errno));
            close(fd);
            return;
        }
    }

    close(fd);
}

void loadFromFile(ContactTable* table) {
    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1) {
        printf("Файл не найден, будет создан новый при сохранении: %s\n", strerror(errno));
        return;
    }

    if (read(fd, &table->contactCount, sizeof(int)) == -1) {
        printf("Ошибка чтения из файла: %s\n", strerror(errno));
        close(fd);
        return;
    }

    for (int i = 0; i < table->contactCount; i++) {
        if (read(fd, &table->contacts[i], sizeof(Contact)) == -1) {
            printf("Ошибка чтения контакта %d: %s\n", i, strerror(errno));
            close(fd);
            return;
        }
    }

    close(fd);
}
