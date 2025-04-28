#include "contact_list.h"
#include <string.h>
#include <stdlib.h>

void initList(ContactList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->contactCount = 0;
}

void addContact(ContactList* list, Contact* contact) {
    ContactNode* newContact = (ContactNode*)malloc(sizeof(ContactNode));
    if (newContact == NULL) {
        return;
    }

    newContact->data = *contact;
    newContact->data.id = list->contactCount + 1;

    ContactNode* current = list->head;
    ContactNode* prev = NULL;

    while (current != NULL && strcmp(current->data.lastName, contact->lastName) < 0) {
        prev = current;
        current = current->next;
    }

    newContact->prev = prev;
    newContact->next = current;

    if (prev != NULL) {
        prev->next = newContact;
    } else {
        list->head = newContact;
    }

    if (current != NULL) {
        current->prev = newContact;
    } else {
        list->tail = newContact;
    }

    list->contactCount++;

    int id = 1;
    current = list->head;
    while (current != NULL) {
        current->data.id = id++;
        current = current->next;
    }
}

int editContact(ContactList* list, int id, Contact* newData) {
    if (isListEmpty(list)) return -1;

    ContactNode* current = list->head;
    while (current != NULL && current->data.id != id) {
        current = current->next;
    }

    if (current == NULL) {
        return -1;
    }

    current->data = *newData;
    current->data.id = id;
    return 1;
}

int deleteContact(ContactList* list, int id) {
    if (isListEmpty(list)) return -1;

    ContactNode* current = list->head;
    while (current != NULL && current->data.id != id) {
        current = current->next;
    }

    if (current == NULL) {
        return -1;
    }

    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
        list->head = current->next;
    }

    if (current->next != NULL) {
        current->next->prev = current->prev;
    } else {
        list->tail = current->prev;
    }

    ContactNode* temp = current->next;
    while (temp != NULL) {
        temp->data.id--;
        temp = temp->next;
    }

    free(current);
    list->contactCount--;
    return 1;
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

int isListEmpty(ContactList* list) {
    return (list == NULL || list->head == NULL);
}
