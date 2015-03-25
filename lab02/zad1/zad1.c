/*
* jeżeli nie podamy argumentu - ma się wyswietlić help jak maja wyglądać argumenty
*
* jak robimy opendir - pamiętać że pierwsze dwa pliki to . i .. - pamiętać żeby je pominąć
*
*
* ryglowanie plików
* -> obowiązujące
* -> nieobowiązujące -  w Linuksie
*       można ustawić w ustawieniach, albo:
*       !!! sprawdzić stan pliku - czy jest ustawiony jakiś rygiel !
*
* */

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include "tools/times.h"
#include "tools/generator.h"
#include "tools/sorter.h"


int main(int argc, char **args) {

    char *name = "generatedFile";
    char *name2 = "generatedFile2";
    int recordSize = 8192;
    int numOfRecords = 1000;

    struct tms prevTimes;
    clock_t prevReal;
    struct tms firstTimes;
    clock_t firstReal;
    prevTimes.tms_stime = -1;

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Generating records file and making the copy.\n");
    generateRecordsFile(name, recordSize, numOfRecords);

    copyFile(name, name2, recordSize, numOfRecords);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Sorting using fwrite/fread.\n");
    bubbleSort(name, recordSize);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);
    printf("Sorting using write/read.\n");
    bubbleSortNoF(name2, recordSize);

    checkTimes(&prevTimes, &prevReal, &firstTimes, &firstReal);

/*
    printFileContents(name, recordSize, numOfRecords);

    printf("\n\n\n");

    printFileContents(name2, recordSize, numOfRecords);
*/

    return 0;
}








