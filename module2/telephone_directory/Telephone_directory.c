#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

// Максимальные длины для строковых полей
#define TABLE_SIZE 512
#define MAX_NAME_LEN 30
#define MAX_JOB_LEN 30
#define MAX_PHONE_LEN 20
#define MAX_EMAIL_LEN 50
#define MAX_URL_LEN 100
#define MAX_SOCIAL_LEN 100
#define FILENAME "book.dat"

// Структура для хранения контакта
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

// Структура для хранения таблицы контактов
typedef struct {
    Contact contacts[TABLE_SIZE]; // Массив контактов
    int contactCount; // Количество контактов
} ContactTable;

// Функции для работы с таблицей контактов
void saveToFile(ContactTable* table);
void loadFromFile(ContactTable* table);
void addContact(ContactTable* table);
int editContact(ContactTable* table, int id);
int deleteContact(ContactTable* table, int id);
void addField(ContactTable* table, int id);
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
    while (1) {
        menuOutput();
        int i;
        scanf_s("%d", &i);
        while (i > 7 || i < 1) {
            printf_s("Сделайте корректный выбор\n");
            scanf_s("%d", &i);
        }
        switch (i)
        {
        case 1:
            addContact(&Table);
            break;
        case 2:
            outTable(&Table);
            printf_s("Введите id контакта: ");
            scanf_s("%d", &_id);
            editContact(&Table, _id);
            break;
        case 3:
            outTable(&Table);
            printf_s("Введите id контакта: ");
            scanf_s("%d", &_id);
            deleteContact(&Table, _id);
            break;
        case 4:
            outTable(&Table);
            break;
        case 5:
            saveToFile(&Table);
            break;
        case 6:
            return 1;
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
    printf_s("Введите имя и фамилию: ");
    scanf_s("%s %s", firstName, MAX_NAME_LEN, lastName, MAX_NAME_LEN);
    Contact contact;
    contact.id = table->contactCount + 1;
    strncpy_s(contact.job, sizeof(contact.job), "", MAX_JOB_LEN);
    strncpy_s(contact.position, sizeof(contact.position), "", MAX_JOB_LEN);
    strncpy_s(contact.phoneNumber, sizeof(contact.phoneNumber), "", MAX_PHONE_LEN);
    strncpy_s(contact.email, sizeof(contact.email), "", MAX_EMAIL_LEN);
    strncpy_s(contact.url, sizeof(contact.url), "", MAX_SOCIAL_LEN);
    strncpy_s(contact.firstName, sizeof(contact.firstName), firstName, MAX_NAME_LEN);
    strncpy_s(contact.lastName, sizeof(contact.lastName), lastName, MAX_NAME_LEN);
    table->contacts[table->contactCount] = contact;
    table->contactCount++;
    printf_s("Контакт %s %s добавлен\n", contact.firstName, contact.lastName);
}

int editContact(ContactTable* table, int id) {
    if (isTableEmpty(table)) return -1;

    printf_s("Что вы хотели бы изменить?\n");
    printf_s("1. Добавить поле\n");
    printf_s("2. Изменить поле\n");
    printf_s("3. Удалить поле\n");
    int i;
    scanf_s("%d", &i);
    switch (i)
    {
    case 1:
        addField(table, id);
        break;
    case 2:
        editField(table, id);
        break;
    case 3:
        deleteField(table, id);
        break;
    default:
        break;
    }

    return 1;
}

void addField(ContactTable* table, int id) {
    printf_s("Какое поле вы хотели бы добавить?\n");
    printf_s("1. Место работы\n");
    printf_s("2. Номер телефона\n");
    printf_s("3. Почтовый адрес\n");
    printf_s("4. Ссылка на социальную сеть\n");
    int i1;
    scanf_s("%d", &i1);
    switch (i1)
    {
    case 1:
        printf_s("Введите место работы: ");
        char job[MAX_JOB_LEN];
        scanf_s("%s", &job, MAX_JOB_LEN);
        strncpy_s(table->contacts[id - 1].job, sizeof(table->contacts[id - 1].job), job, MAX_JOB_LEN);
        printf_s("Введите должность: ");
        char position[MAX_JOB_LEN];
        scanf_s("%s", &position, MAX_JOB_LEN);
        strncpy_s(table->contacts[id - 1].position, sizeof(table->contacts[id - 1].position), position, MAX_JOB_LEN);
        break;
    case 2:
        printf_s("Введите номер телефона: ");
        char phoneNumber[MAX_PHONE_LEN];
        scanf_s("%s", &phoneNumber, MAX_PHONE_LEN);
        strncpy_s(table->contacts[id - 1].phoneNumber, sizeof(table->contacts[id - 1].phoneNumber), phoneNumber, MAX_PHONE_LEN);
        break;
    case 3:
        printf_s("Введите почтовый адрес: ");
        char email[MAX_EMAIL_LEN];
        scanf_s("%s", &email, MAX_EMAIL_LEN);
        strncpy_s(table->contacts[id - 1].email, sizeof(table->contacts[id - 1].email), email, MAX_EMAIL_LEN);
        break;
    case 4:
        printf_s("Введите ссылку на соцсеть: ");
        char url[MAX_SOCIAL_LEN];
        scanf_s("%s", &url, MAX_SOCIAL_LEN);
        strncpy_s(table->contacts[id - 1].url, sizeof(table->contacts[id - 1].url), url, MAX_SOCIAL_LEN);
        break;
    default:
        break;
    }
}

void editField(ContactTable* table, int id) {
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
    switch (i)
    {
    case 1:
        printf_s("Введите новое имя: ");
        char firstName[MAX_NAME_LEN];
        scanf_s("%s", firstName, MAX_NAME_LEN);
        strncpy_s(table->contacts[id - 1].firstName, sizeof(table->contacts[id - 1].firstName), firstName, MAX_NAME_LEN);
        break;
    case 2:
        printf_s("Введите новую фамилию: ");
        char lastName[MAX_NAME_LEN];
        scanf_s("%s", lastName, MAX_NAME_LEN);
        strncpy_s(table->contacts[id - 1].lastName, sizeof(table->contacts[id - 1].lastName), lastName, MAX_NAME_LEN);
        break;
    case 3:
        printf_s("Введите новое место работы: ");
        char job[MAX_JOB_LEN];
        scanf_s("%s", &job, MAX_JOB_LEN);
        strncpy_s(table->contacts[id - 1].job, sizeof(table->contacts[id - 1].job), job, MAX_JOB_LEN);
        break;
    case 4:
        printf_s("Введите новую должность: ");
        char position[MAX_JOB_LEN];
        scanf_s("%s", &position, MAX_JOB_LEN);
        strncpy_s(table->contacts[id - 1].position, sizeof(table->contacts[id - 1].position), position, MAX_JOB_LEN);
        break;
    case 5:
        printf_s("Введите новый номер телефона: ");
        char phoneNumber[MAX_PHONE_LEN];
        scanf_s("%s", &phoneNumber, MAX_PHONE_LEN);
        strncpy_s(table->contacts[id - 1].phoneNumber, sizeof(table->contacts[id - 1].phoneNumber), phoneNumber, MAX_PHONE_LEN);
        break;
    case 6:
        printf_s("Введите новый почтовый адрес: ");
        char email[MAX_EMAIL_LEN];
        scanf_s("%s", &email, MAX_EMAIL_LEN);
        strncpy_s(table->contacts[id - 1].email, sizeof(table->contacts[id - 1].email), email, MAX_EMAIL_LEN);
        break;
    case 7:
        printf_s("Введите новую ссылку на соцсеть: ");
        char url[MAX_SOCIAL_LEN];
        scanf_s("%s", &url, MAX_SOCIAL_LEN);
        strncpy_s(table->contacts[id - 1].url, sizeof(table->contacts[id - 1].url), url, MAX_SOCIAL_LEN);
        break;
    default:
        break;
    }
}

void deleteField(ContactTable* table, int id) {
    printf_s("Какое поле вы хотели бы удалить?\n");
    printf_s("1. Место работы\n");
    printf_s("2. Номер телефона\n");
    printf_s("3. Почтовый адрес\n");
    printf_s("4. Ссылка на социальную сеть\n");
    int i;
    scanf_s("%d", &i);
    switch (i)
    {
    case 1:
        strncpy_s(table->contacts[id - 1].job, sizeof(table->contacts[id - 1].job), "", MAX_JOB_LEN);
        strncpy_s(table->contacts[id - 1].position, sizeof(table->contacts[id - 1].position), "", MAX_JOB_LEN);
        break;
    case 2:
        strncpy_s(table->contacts[id - 1].phoneNumber, sizeof(table->contacts[id - 1].phoneNumber), "", MAX_PHONE_LEN);
        break;
    case 3:
        strncpy_s(table->contacts[id - 1].email, sizeof(table->contacts[id - 1].email), "", MAX_EMAIL_LEN);
        break;
    case 4:
        strncpy_s(table->contacts[id - 1].url, sizeof(table->contacts[id - 1].url), "", MAX_SOCIAL_LEN);
        break;
    default:
        break;
    }
}

int isTableEmpty(ContactTable* table) {
    if (table == NULL) return 1;
    return 0;
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
    // Контакт не найден
    if (index == -1) {
        return index;
    }

    memset(&table->contacts[index], 0, sizeof(Contact));

  
    for (int i = index; i < table->contactCount - 1; i++) {
        table->contacts[i] = table->contacts[i + 1];
    }
    table->contactCount--;
    return 1;
}

// Функция для сохранения данных в файл
void saveToFile(ContactTable* table) {
    FILE* file;
    if (fopen_s(&file, FILENAME, "wb") != 0) {
        printf("Ошибка открытия файла для записи\n");
        return;
    }

    // Сначала записываем количество контактов
    fwrite(&table->contactCount, sizeof(int), 1, file);

    // Затем записываем каждый контакт
    for (int i = 0; i < table->contactCount; i++) {
        fwrite(&table->contacts[i], sizeof(Contact), 1, file);
    }

    fclose(file);
}

// Функция для загрузки данных из файла
void loadFromFile(ContactTable* table) {
    FILE* file;
    if (fopen_s(&file, FILENAME, "rb") != 0) {
        printf("Файл не найден, будет создан новый при сохранении\n");
        return;
    }

    // Считываем количество контактов
    fread(&table->contactCount, sizeof(int), 1, file);

    // Считываем каждый контакт
    for (int i = 0; i < table->contactCount; i++) {
        fread(&table->contacts[i], sizeof(Contact), 1, file);
    }

    fclose(file);
}
