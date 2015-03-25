#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sorter.h"

void bubbleSortNoF(char *fileName, int recordSize) {

    int source = open(fileName, O_RDWR);
    if (source == -1) {
        printf("Source file not found.\n");
        exit(1);
    }

    char rec1[recordSize];
    char rec2[recordSize];
    int i;
    int sorted = 0;
    ssize_t elementsRead;
    ssize_t elementsRead2;

    while (!sorted) {
        sorted = 1;
        i = 0;
        elementsRead = 1;
        elementsRead2 = 1;

        while (elementsRead != 0 && elementsRead2 != 0) {
            lseek(source, i * (recordSize * sizeof(char)), SEEK_SET);
            elementsRead = read(source, rec1, recordSize * sizeof(char));
            elementsRead2 = read(source, rec2, recordSize * sizeof(char));

            if (elementsRead == -1 || elementsRead2 == -1) {
                printf("Error while reading file %s.", fileName);
                exit(16);
            }
            if (elementsRead != 0 && elementsRead2 != 0 && rec1[0] > rec2[0]) {
                lseek(source, i * (recordSize * sizeof(char)), SEEK_SET);

                write(source, rec2, recordSize * sizeof(char));
                write(source, rec1, recordSize * sizeof(char));
                sorted = 0;
            }
            i++;
        }
        lseek(source, 0, SEEK_SET);
    }
}


void bubbleSort(char *fileName, int recordSize) {

    FILE *source;
    source = fopen(fileName, "r+");

    if (source == NULL) {
        printf("Source file not found.\n");
        exit(1);
    }

    char rec1[recordSize];
    char rec2[recordSize];

    int elementNumber = 0;
    int sorted = 0;
    while (!sorted) {
        sorted = 1;
        elementNumber = 0;
        while (!feof(source)) {
            fseek(source, elementNumber * (recordSize * sizeof(char)), SEEK_SET);

            size_t elementsRead = fread(rec1, recordSize * sizeof(char), 1, source);
            size_t elementsRead2 = fread(rec2, recordSize * sizeof(char), 1, source);

            if (elementsRead != 1 || elementsRead2 != 1) {
                if (!feof(source)) {
                    printf("Error while reading file %s.", fileName);
                    exit(16);
                } else {
                    continue;
                }
            }

            if (rec1[0] > rec2[0]) {
                fseek(source, elementNumber * (recordSize * sizeof(char)), SEEK_SET);

                fwrite(rec2, recordSize * sizeof(char), 1, source);
                fwrite(rec1, recordSize * sizeof(char), 1, source);
                sorted = 0;
            }
            elementNumber++;
        }
        fseek(source, 0, SEEK_SET);
    }
}
