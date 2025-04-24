#include "telephone_directory.h"
#include <stdio.h>
#include <locale.h>

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
        scanf("%d", &i);
        while (i > 7 || i < 1) {
            printf("Сделайте корректный выбор\n");
            scanf("%d", &i);
        }
        switch (i) {
        case 1:
            addContact(&list);
            break;
        case 2:
            outList(&list);
            printf("Введите id контакта: ");
            scanf("%d", &_id);
            editContact(&list, _id);
            break;
        case 3:
            outList(&list);
            printf("Введите id контакта: ");
            scanf("%d", &_id);
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
    printf("1. Добавить контакт\n");
    printf("2. Изменить контакт\n");
    printf("3. Удалить контакт\n");
    printf("4. Вывод телефонной книги\n");
    printf("5. Сохранить телефонную книгу\n");
    printf("6. Выйти из программы\n");
}
