#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <signal.h>

#include "common.h"


FILE *fifo;

void closeFifo(int signal);

void prepareFifo(char *fifoFilename);


int main(int argc, char *argv[]) {

    char *fifoFilename = getFifoFilename(argc, argv);
    prepareFifo(fifoFilename);
    signal(SIGTERM, closeFifo);
    printf("Started reading...\nTo exit use Ctrl+C\n");

    char message[MESSAGE_MAX_LEN];

    while (1) {
        fifo = openFifo(fifoFilename, "r");
        fgets(message, MESSAGE_MAX_LEN, fifo);

        char *nowAsString = getNowAsString();
        printf("recieved: %s, %s\n", nowAsString, message);

        free(nowAsString);
        fclose(fifo);
        fifo = NULL;
    }
}

void closeFifo(int signal) {
    if (fifo != NULL) {
        fclose(fifo);
        fifo = NULL;
    }
}

void prepareFifo(char *fifoFilename) {
    mode_t backup = umask(0);
    mkfifo(fifoFilename, 0666); // (mode & ~umask)!
    umask(backup);
}

