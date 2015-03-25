#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "generator.h"

void generateRecordsFile(char *fileName, int recordSize, int numOfRecords) {

    FILE *file;

    file = fopen(fileName, "w");
    if (file == NULL) {
        printf("File not found.\n");
        exit(1);
    }

    srand(time(NULL));
    char *record = malloc(recordSize * sizeof(char));
    int i, j;
    for (j = 0; j < numOfRecords; ++j) {
        for (i = 0; i < recordSize; ++i) {
            record[i] = (char) (rand() % 255);
        }
        fwrite(record, recordSize * sizeof(char), 1, file);
    }
    fclose(file);
}


void copyFile(char *sourceFileName, char *destFileName, int lineLength, int numOfRecords) {

    FILE *source, *target;

    source = fopen(sourceFileName, "r");
    if (source == NULL) {
        printf("Source not found.\n");
        exit(1);
    }

    target = fopen(destFileName, "w");
    if (target == NULL) {
        fclose(source);
        printf("Target not found.\n");
        exit(1);
    }

    int i;
    char line[lineLength];
    for (i = 0; i < numOfRecords; ++i) {
        if (fread(line, lineLength * sizeof(char), 1, source) != 1) {
            printf("Error while reading file %s.", sourceFileName);
            exit(1);
        }

        if (fwrite(line, lineLength * sizeof(char), 1, target) != 1) {
            printf("Error while writing to file %s.", destFileName);
            exit(1);
        }
    }
    printf("File copied successfully.\n");

    fclose(source);
    fclose(target);
}


void printFileContents(char *fileName, int recordSize, int numOfRecords) {

    FILE *source;
    source = fopen(fileName, "r+");
    if (source == NULL) {
        printf("Source file not found.\n");
        exit(1);
    }

    int i, j;
    char line[recordSize];
    for (i = 0; i < numOfRecords; ++i) {
        if (fread(line, recordSize * sizeof(char), 1, source) != 1) {
            printf("Error while reading file %s.", fileName);
            exit(1);
        }
        for (j = 0; j < recordSize; ++j) {
            printf("%d ", line[j]);
        }
        printf("\n");
    }
}