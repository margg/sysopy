#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

const int NO_FIFO_PATH = 2;
const int NO_FIFO = 3;
const unsigned MESSAGE_MAX_LEN = 150;


char *getNowAsString() {
    time_t currentTime;
    time(&currentTime);
    struct tm *tmStruct = localtime(&currentTime);

    char *result = malloc(sizeof(char) * 9);
    strftime(result, 9, "%H:%M:%S", tmStruct);
    return result;
}

char *getFifoFilename(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: \n\tprogram <fifo path>");
        exit(NO_FIFO_PATH);
    }
    return argv[1];
}

FILE *openFifo(char *fifoFilename, char *mode) {
    FILE *fifo = fopen(fifoFilename, mode);
    if (fifo == NULL) {
        printf("Node (\"%s\") does not exist\n", fifoFilename);
        exit(NO_FIFO);
    }
    return fifo;
}


