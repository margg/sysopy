#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>
#include "../zad1/address_list.h"

#ifdef DYNAMIC
#include<dlfcn.h>
#endif


void printList(struct List *list);

void checkTimes(struct tms *prevTimes, clock_t *prevReal, struct tms *firstTimes, clock_t *firstReal);

int main(int argc, char **args) {

#ifdef DYNAMIC
    void* library = dlopen("libaddress_list_shared.so", RTLD_LAZY);

    struct List* (*createList)();
	createList = dlsym(library, "createList");

    int* (*addElement)(struct Data data, struct List *list);
    addElement = dlsym(library, "addElement");

    void* (*deleteElement)(struct ListElement *el, struct List *list);
    deleteElement = dlsym(library, "deleteElement");

    struct ListElement* (*findElement)(struct Data data, struct List *list);
    findElement = dlsym(library, "findElement");

    struct List* (*sortByName)(struct List *list);
    sortByName = dlsym(library, "sortByName");

    void* (*deleteList)(struct List *list);
    deleteList = dlsym(library, "deleteList");

#endif


    char *names[] = {"Anna", "Barney", "Cecille", "Duncan", "Elize", "Frodo", "Gertrude",
            "Hipolito", "Inez", "Johnny", "Kay", "Luke", "Monique", "Nico", "Oprah", "Peter",
            "Raquel", "Stan", "Trinity", "Uriel", "Vivien", "Warwick", "Zenoby"};
    char *surnames[] = {"Aaron", "Abandoned", "Beforeable", "Bundy", "Charizard", "Chomsky", "Dreyfuse",
            "Ernnie", "Foldable", "Growth", "Huntery", "Jopp", "Kustard", "Leopard", "Lohn", "Mummond",
            "Osterrich", "Pueblo", "Robbinsky", "Sequel", "Straw", "Tintin", "Wicked"};

    int NUMBER_OF_NAMES = 23;
    int NUMBER_OF_SURNAMES = 23;
    int LIST_SIZE = 10000;

    struct tms prevTimes;
    clock_t prevReal;
    struct tms firstTimes;
    clock_t firstReal;

    prevTimes.tms_stime = -1;

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Creating the list of %d elements.\n", LIST_SIZE);

    struct List *list = createList();

    struct Data data;

    data.name = "Joe";
    data.surname = "Black";
    data.birthDate = "10.03.1992";
    data.email = "Black@b.pl";
    data.phone = "79456123";
    data.street = "Krakowska";
    data.number = "10A";
    data.postCode = "45-263";
    data.city = "Krakow";
    data.country = "Poland";

    srand(time(NULL));
    int i;
    for (i = 0; i < LIST_SIZE; i++) {
        data.name = names[rand() % NUMBER_OF_NAMES];
        data.surname = surnames[rand() % NUMBER_OF_SURNAMES];

        addElement(data, list);
    }

//    printList(list);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Finding contact in the list (name: %s %s)\n", data.name, data.surname);

    struct ListElement *el = findElement(data, list);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Deleting contact from the list.\n");

    deleteElement(el, list);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Sorting the list.\n");

    struct List *list2 = sortByName(list);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Deleting the list.\n");
    deleteList(list2);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Done!\n");

    return 0;
}

#define CLK sysconf(_SC_CLK_TCK)

void checkTimes(struct tms *prevTimes, clock_t *prevReal,
        struct tms *firstTimes, clock_t *firstReal) {

    struct tms now;
    times(&now);
    clock_t nowReal = clock();
    if (prevTimes->tms_stime == -1) {
        *firstTimes = now;
        *firstReal = nowReal;
    } else {
        printf("\n\tFrom the first check:\t\tR %.6f\tS %.6f\tU %.6f",
                ((double) (nowReal - *(firstReal))) / CLOCKS_PER_SEC,
                ((double) (now.tms_stime - firstTimes->tms_stime)) / CLK,
                ((double) (now.tms_utime - firstTimes->tms_utime)) / CLK);

        printf("\n\tFrom the previous check:\tR %.6f\tS %.6f\tU %.6f",
                ((double) (nowReal - *(prevReal))) / CLOCKS_PER_SEC,
                ((double) (now.tms_stime - prevTimes->tms_stime)) / CLK,
                ((double) (now.tms_utime - prevTimes->tms_utime)) / CLK);
    }
    printf("\n\tTime:\t\t\t\tR %.6f\tS %.6f\tU %.6f\n\n",
            ((double) nowReal) / CLOCKS_PER_SEC,
            ((double) now.tms_stime) / CLK,
            ((double) now.tms_utime) / CLK);
    *prevReal = nowReal;
    *prevTimes = now;

}

void printList(struct List *list) {

    if (list == NULL) {
        printf("Null list.");
        return;
    }

    struct ListElement *first = list->head;

    while (first != NULL) {
        printf("Element: %s %s\n", first->data->surname, first->data->name);
        first = first->next;
    }
}