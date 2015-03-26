#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include "tools/times.h"
#include "tools/generator.h"
#include "tools/sorter.h"


int main(int argc, char **argv) {

    char *name = "generatedFile";
    char *copy1 = "generatedFile2";
    int recordSize = 4096;
    int numOfRecords = 200;

    if(argc == 3) {
        recordSize = atoi(argv[1]);
        numOfRecords = atoi(argv[2]);
    }

    struct tms prevTimes;
    clock_t prevReal;
    struct tms firstTimes;
    clock_t firstReal;
    prevTimes.tms_stime = -1;

    printf("Generating records file and making the copy.\n");
    generateRecordsFile(name, recordSize, numOfRecords);
    copyFile(name, copy1, recordSize, numOfRecords);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Sorting using fwrite/fread.\n");
    bubbleSort(name, recordSize);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Sorting using write/read.\n");
    bubbleSortNoF(copy1, recordSize);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);

    return 0;
}








