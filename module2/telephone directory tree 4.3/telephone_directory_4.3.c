#define _CRT_SECURE_NO_WARNINGS
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

// Структура контакта
typedef struct {
    int id; // Уникальный идентификатор контакта
    char firstName[MAX_NAME_LEN]; // Имя (обязательное поле)
    char lastName[MAX_NAME_LEN]; // Фамилия (обязательное поле)
    char job[MAX_JOB_LEN]; // Место работы
    char position[MAX_JOB_LEN]; // Должность
    char phoneNumber[MAX_PHONE_LEN]; // Номер телефона
    char email[MAX_EMAIL_LEN]; // Адрес электронной почты
    char url[MAX_URL_LEN]; // Ссылка на соцсеть
} Contact;

// Узел Tree-дерева
typedef struct TreeNode {
    Contact contact;
    struct TreeNode* left;
    struct TreeNode* right;
    int height;
} TreeNode;

// Функции для работы с Tree-деревом
TreeNode* createNode(Contact contact);
int height(TreeNode* node);
int getBalance(TreeNode* node);
TreeNode* rightRotate(TreeNode* y);
TreeNode* leftRotate(TreeNode* x);
TreeNode* insertNode(TreeNode* node, Contact contact);
TreeNode* minValueNode(TreeNode* node);
TreeNode* deleteNode(TreeNode* root, int id);
void inOrderTraversal(TreeNode* root);
TreeNode* findNode(TreeNode* root, int id);
void freeTree(TreeNode* root);
void updateIdsAfterDeletion(TreeNode* root, int* currentId);

// Функции для работы с телефонной книгой
void saveToFileHelper(FILE* file, TreeNode* node);
void saveToFile(TreeNode* root);
TreeNode* loadFromFile();
void addContact(TreeNode** root);
void editContact(TreeNode* root);
void deleteContact(TreeNode** root);
void printContact(Contact contact);
void outTable(TreeNode* root);
void menuOutput();

int nextId = 1;

int main() {
    setlocale(LC_ALL, "Russian");
    TreeNode* root = loadFromFile();

    outTable(root);

    while (1) {
        menuOutput();
        int i;
        scanf("%d", &i);
        while (i > 6 || i < 1) {
            printf("Сделайте корректный выбор\n");
            scanf("%d", &i);
        }
        switch (i) {
        case 1:
            addContact(&root);
            break;
        case 2:
            outTable(root);
            editContact(root);
            break;
        case 3:
            outTable(root);
            deleteContact(&root);
            break;
        case 4:
            outTable(root);
            break;
        case 5:
            saveToFile(root);
            break;
        case 6:
            freeTree(root);
            return 0;
        }
    }
}

// Создание нового узла
TreeNode* createNode(Contact contact) {
    TreeNode* node = malloc(sizeof(TreeNode));
    node->contact = contact;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

// Получение высоты узла
int height(TreeNode* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// Получение баланс-фактора узла
int getBalance(TreeNode* node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

// Правый поворот
TreeNode* rightRotate(TreeNode* y) {
    TreeNode* x = y->left;
    TreeNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

// Левый поворот
TreeNode* leftRotate(TreeNode* x) {
    TreeNode* y = x->right;
    TreeNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

// Вставка узла в дерево
TreeNode* insertNode(TreeNode* node, Contact contact) {
    if (node == NULL)
        return createNode(contact);

    if (contact.id < node->contact.id)
        node->left = insertNode(node->left, contact);
    else if (contact.id > node->contact.id)
        node->right = insertNode(node->right, contact);
    else
        return node; // Одинаковые ключи не допускаются

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    // Левое дерево перевешивает
    if (balance > 1 && contact.id < node->left->contact.id)
        return rightRotate(node);

    // Правое дерево перевешивает
    if (balance < -1 && contact.id > node->right->contact.id)
        return leftRotate(node);

    // Левое дерево перевешивает, но происходит вставка правого потомка
    if (balance > 1 && contact.id > node->left->contact.id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Правое дерево перевешивает, но происходит вставка левого потомка
    if (balance < -1 && contact.id < node->right->contact.id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Поиск узла с минимальным значением
TreeNode* minValueNode(TreeNode* node) {
    TreeNode* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// Удаление узла
TreeNode* deleteNode(TreeNode* root, int id) {
    if (root == NULL)
        return root;

    if (id < root->contact.id)
        root->left = deleteNode(root->left, id);
    else if (id > root->contact.id)
        root->right = deleteNode(root->right, id);
    else {
        if ((root->left == NULL) || (root->right == NULL)) {
            TreeNode* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            }
            else
                *root = *temp;
            free(temp);
        }
        else {
            TreeNode* temp = minValueNode(root->right);
            root->contact = temp->contact;
            root->right = deleteNode(root->right, temp->contact.id);
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    // Левый левый случай
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // Левый правый случай
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Правый правый случай
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // Правый левый случай
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Обновление ID после удаления
void updateIdsAfterDeletion(TreeNode* root, int* currentId) {
    if (root == NULL) return;

    updateIdsAfterDeletion(root->left, currentId);
    root->contact.id = (*currentId)++;
    updateIdsAfterDeletion(root->right, currentId);
}

// Центрированный обход дерева (для вывода)
void inOrderTraversal(TreeNode* root) {
    if (root != NULL) {
        inOrderTraversal(root->left);
        printContact(root->contact);
        inOrderTraversal(root->right);
    }
}

// Поиск узла по ID
TreeNode* findNode(TreeNode* root, int id) {
    if (root == NULL || root->contact.id == id)
        return root;

    if (root->contact.id < id)
        return findNode(root->right, id);

    return findNode(root->left, id);
}

// Освобождение памяти дерева
void freeTree(TreeNode* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// Сохранение дерева в файл (вспомогательная функция)
void saveToFileHelper(FILE* file, TreeNode* node) {
    if (node == NULL) return;

    // Сохраняем контакт
    fwrite(&node->contact, sizeof(Contact), 1, file);

    // Рекурсивно сохраняем левое и правое поддеревья
    saveToFileHelper(file, node->left);
    saveToFileHelper(file, node->right);
}

// Сохранение дерева в файл
void saveToFile(TreeNode* root) {
    FILE* file;
    if (fopen_s(&file, FILENAME, "wb") != 0) {
        printf("Ошибка открытия файла для записи\n");
        return;
    }

    // Сначала сохраняем nextId, следующий доступный id
    fwrite(&nextId, sizeof(int), 1, file);

    // Затем сохраняем дерево
    saveToFileHelper(file, root);

    fclose(file);
    printf("Данные сохранены в файл\n");
}

// Загрузка дерева из файла
TreeNode* loadFromFile() {
    FILE* file;
    if (fopen_s(&file, FILENAME, "rb") != 0) {
        printf("Файл не найден, будет создан новый при сохранении\n");
        return NULL;
    }

    // Читаем nextId
    if (fread(&nextId, sizeof(int), 1, file) != 1) {
        nextId = 1;
    }

    TreeNode* root = NULL;
    Contact contact;

    // Читаем контакты пока не достигнем конца файла
    while (fread(&contact, sizeof(Contact), 1, file)) {
        root = insertNode(root, contact);
    }

    fclose(file);
    return root;
}

// Добавление контакта
void addContact(TreeNode** root) {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    printf("Введите имя и фамилию: ");
    scanf_s("%s %s", firstName, lastName);

    Contact contact;
    contact.id = nextId++;
    strncpy(contact.firstName, firstName, MAX_NAME_LEN);
    strncpy(contact.lastName, lastName, MAX_NAME_LEN);
    strncpy(contact.job, "", MAX_JOB_LEN);
    strncpy(contact.position, "", MAX_JOB_LEN);
    strncpy(contact.phoneNumber, "", MAX_PHONE_LEN);
    strncpy(contact.email, "", MAX_EMAIL_LEN);
    strncpy(contact.url, "", MAX_URL_LEN);

    *root = insertNode(*root, contact);
    printf("Контакт %s %s добавлен (ID: %d)\n", contact.firstName, contact.lastName, contact.id);
}

// Редактирование контакта
void editContact(TreeNode* root) {
    if (root == NULL) {
        printf("Телефонная книга пуста\n");
        return;
    }

    printf("Введите ID контакта для редактирования: ");
    int id;
    scanf("%d", &id);

    TreeNode* node = findNode(root, id);
    if (node == NULL) {
        printf("Контакт с ID %d не найден\n", id);
        return;
    }

    printf("Что вы хотели бы изменить?\n");
    printf("1. Имя\n");
    printf("2. Фамилия\n");
    printf("3. Место работы\n");
    printf("4. Должность\n");
    printf("5. Номер телефона\n");
    printf("6. Почтовый адрес\n");
    printf("7. Ссылка на социальную сеть\n");

    int choice;
    scanf("%d", &choice);

    switch (choice) {
    case 1:
        printf("Введите новое имя: ");
        scanf("%s", node->contact.firstName);
        break;
    case 2:
        printf("Введите новую фамилию: ");
        scanf("%s", node->contact.lastName);
        break;
    case 3:
        printf("Введите место работы: ");
        scanf("%s", node->contact.job);
        break;
    case 4:
        if (strlen(node->contact.job) == 0) {
            printf("Сначала введите место работы: ");
            scanf("%s", node->contact.job);
        }
        printf("Введите должность: ");
        scanf("%s", node->contact.position);
        break;
    case 5:
        printf("Введите номер телефона: ");
        scanf("%s", node->contact.phoneNumber);
        break;
    case 6:
        printf("Введите почтовый адрес: ");
        scanf("%s", node->contact.email);
        break;
    case 7:
        printf("Введите ссылку на соцсеть: ");
        scanf("%s", node->contact.url);
        break;
    default:
        printf("Неверный выбор\n");
    }

    printf("Контакт обновлен\n");
}

// Удаление контакта
void deleteContact(TreeNode** root) {
    if (*root == NULL) {
        printf("Телефонная книга пуста\n");
        return;
    }

    printf("Введите ID контакта для удаления: ");
    int id;
    scanf("%d", &id);

    *root = deleteNode(*root, id);

    // Обновляем ID оставшихся контактов
    int currentId = 1;
    updateIdsAfterDeletion(*root, &currentId);
    nextId = currentId;

    printf("Контакт с ID %d удален, ID остальных контактов обновлены\n", id);
}

// Печать контакта
void printContact(Contact contact) {
    printf("| %2d | %-11s | %-12s | %-22s | %-22s | %-16s | %-36s | %-42s |\n",
        contact.id, contact.firstName, contact.lastName,
        contact.job, contact.position, contact.phoneNumber,
        contact.email, contact.url);
}

// Вывод всей таблицы
void outTable(TreeNode* root) {
    if (root == NULL) {
        printf("Телефонная книга пуста\n");
        return;
    }
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| ID | Имя         | Фамилия     | Работа                  | Должность              | Телефон          | Email                                | Соцсеть                                    |\n");
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    inOrderTraversal(root);
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

// Вывод меню
void menuOutput() {
    printf("\n1. Добавить контакт\n");
    printf("2. Изменить контакт\n");
    printf("3. Удалить контакт\n");
    printf("4. Вывод телефонной книги\n");
    printf("5. Сохранить телефонную книгу\n");
    printf("6. Выйти из программы\n");
    printf("Выберите действие: ");
}
