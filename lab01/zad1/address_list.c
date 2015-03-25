#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/string2.h>
#include "address_list.h"


struct List *sortByName(struct List *list) {

    if (list == NULL || list->head == NULL || list->head->next == NULL) {
        return list;
    }

    struct List *newList = malloc(sizeof(struct List));

    while (list->head != NULL) {
        struct ListElement *maxEl = findMax(list);
        unpinElement(maxEl, list);
        pinFront(maxEl, newList);
    }

    return newList;
}

void pinFront(struct ListElement *el, struct List *list) {

    if (list == NULL || el == NULL) {
        return;
    }

    el->next = list->head;
    el->prev = NULL;

    if (list->head == NULL) {
        list->head = el;
        list->tail = el;
        list->count = 1;
        return;
    }

    list->head->prev = el;
    list->head = el;
    list->count = list->count + 1;
}

struct ListElement *unpinElement(struct ListElement *el, struct List *list) {

    if (findElement(*(el->data), list) == NULL) {
        return NULL;
    }

    if (el->prev == NULL) {  // el == HEAD
        list->head = el->next;
        if (list->head != NULL) {
            list->head->prev = NULL;
        }
    } else {
        el->prev->next = el->next;
    }

    if (el->next == NULL) {  // el == TAIL
        list->tail = el->prev;
        if (list->tail != NULL) {
            list->tail->next = NULL;
        }
    } else {
        el->next->prev = el->prev;
    }

    list->count = list->count - 1;

    return el;
}

struct ListElement *findMax(struct List *list) {

    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    struct ListElement *first = list->head;
    struct ListElement *maxEl = first;

    while (first != NULL) {
        int cmp = compareData(first->data, maxEl->data);
        if (cmp > 0) {
            maxEl = first;
        }
        first = first->next;
    }
    return maxEl;
}

int compareData(struct Data *data1, struct Data *data2) {
    int cmp = strcmp(data1->surname, data2->surname);
    if (cmp == 0) {
        cmp = strcmp(data1->name, data2->name);
    }
    return cmp;
}

void deleteList(struct List *list) {

    if (list == NULL) {
        return;
    }

    struct ListElement *first = list->head;

    while (first != NULL) {
        struct ListElement *toBeDeleted = first;
        first = first->next;

        free(toBeDeleted->data);
        free(toBeDeleted);
    }
    free(list);
}

void deleteElement(struct ListElement *el, struct List *list) {
    unpinElement(el, list);
    free(el->data);
    free(el);
}

struct ListElement *findElement(struct Data data, struct List *list) {

    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    struct ListElement *first = list->head;

    while (first != NULL) {
        if (compareData(first->data, &data) == 0) {
            return first;
        }
        first = first->next;
    }
    return NULL;
}

struct List *createList() {

    struct List *list = malloc(sizeof(struct List));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return list;
}

int addElement(struct Data data, struct List *list) {

    if (list == NULL) {
        return 1;
    }

    if (!validateData(&data)) {
        printf("Validation failed!");
        return 1;
    }

    struct Data *d = malloc(sizeof(struct Data));

    copyData(data, d);

    struct ListElement *el = malloc(sizeof(struct ListElement));
    el->data = d;

    el->next = NULL;
    el->prev = list->tail;
    if (list->tail != NULL) {
        list->tail->next = el;
    }
    list->count = list->count + 1;
    list->tail = el;

    if (list->head == NULL) {
        list->head = el;
    } else if (list->head->next == NULL) {
        list->head->next = el;
    }
    return 0;
}


void copyData(struct Data sourceData, struct Data *destData) {

    destData->name = strdup(sourceData.name);
    destData->surname = strdup(sourceData.surname);
    destData->birthDate = strdup(sourceData.birthDate);
    destData->email = strdup(sourceData.email);
    destData->phone = strdup(sourceData.phone);

    destData->street = strdup(sourceData.street);
    destData->number = strdup(sourceData.number);
    destData->postCode = strdup(sourceData.postCode);
    destData->city = strdup(sourceData.city);
    destData->country = strdup(sourceData.country);
}

int validateData(struct Data *data) {

    if (data->name == NULL || strlen(data->name) == 0) {
        data->name = "(no name)";
    }
    if (data->surname == NULL || strlen(data->surname) == 0) {
        data->surname = "(no surname)";
    }
    if (data->email == NULL || strlen(data->email) == 0) {
        data->email = "(no email)";
    }
    if (data->birthDate == NULL || strlen(data->birthDate) == 0) {
        data->birthDate = "(no birthDate)";
    }
    if (data->phone == NULL || strlen(data->phone) == 0) {
        data->phone = "(no phone)";
    }
    if (data->street == NULL || strlen(data->street) == 0) {
        data->street = "(no street)";
    }
    if (data->number == NULL || strlen(data->number) == 0) {
        data->number = "(no number)";
    }
    if (data->postCode == NULL || strlen(data->postCode) == 0) {
        data->postCode = "(no postCode)";
    }
    if (data->city == NULL || strlen(data->city) == 0) {
        data->city = "(no city)";
    }
    if (data->country == NULL || strlen(data->country) == 0) {
        data->country = "(no country)";
    }

    return 1;   // todo
}
