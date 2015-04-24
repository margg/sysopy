#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"

char const *FORMAT = "senderPid: %d, created: %s, message: %s";

int main(int argc, char *argv[]) {

    char *fifoFilename = getFifoFilename(argc, argv);
    char message[MESSAGE_MAX_LEN];
    char line[MESSAGE_MAX_LEN];
    int pid = getpid();


    printf("Started writing...\nTo exit send \"exit\"...\n");
    while (strcmp(line, "exit\n")) {
        printf(">> ");
        fgets(line, MESSAGE_MAX_LEN, stdin);

        FILE *fifo = openFifo(fifoFilename, "w");
        char *nowAsString = getNowAsString();

        printf(FORMAT, pid, nowAsString, line);
        sprintf(message, FORMAT, pid, nowAsString, line);

        free(nowAsString);
        fputs(message, fifo);
        fclose(fifo);
    }

    return 0;
}

