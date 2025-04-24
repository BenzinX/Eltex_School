#ifndef CONTACT_LIST_H
#define CONTACT_LIST_H

#define MAX_NAME_LEN 30
#define MAX_JOB_LEN 30
#define MAX_PHONE_LEN 20
#define MAX_EMAIL_LEN 50
#define MAX_URL_LEN 100

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

typedef struct ContactNode {
    Contact data;
    struct ContactNode* prev;
    struct ContactNode* next;
} ContactNode;

typedef struct {
    ContactNode* head;
    ContactNode* tail;
    int contactCount;
} ContactList;

void initList(ContactList* list);
void addContact(ContactList* list, Contact* contact);
int editContact(ContactList* list, int id, Contact* newData);
int deleteContact(ContactList* list, int id);
void freeList(ContactList* list);
int isListEmpty(ContactList* list);

#endif
