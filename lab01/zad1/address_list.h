#pragma once

struct List {
    struct ListElement *head;
    struct ListElement *tail;

    int count;
};

struct ListElement {
    struct Data *data;

    struct ListElement *next;
    struct ListElement *prev;

};

struct Data {

    char *name;
    char *surname;
    char *birthDate;
    char *email;
    char *phone;

    char *street;
    char *number;
    char *postCode;
    char *city;
    char *country;
};


struct List *createList();

int addElement(struct Data data, struct List *list);

void deleteElement(struct ListElement *el, struct List *list);

struct ListElement *findElement(struct Data data, struct List *list);

struct List * sortByName(struct List *list);

void deleteList(struct List *list);


int compareData(struct Data *data1, struct Data *data2);

int validateData(struct Data *data);

void copyData(struct Data sourceData, struct Data *destData);

struct ListElement *unpinElement(struct ListElement *el, struct List *list);

struct ListElement *findMax(struct List *list);

void pinFront(struct ListElement *el, struct List *list);
