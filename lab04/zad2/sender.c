#include<stdio.h>
#include<signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>

int SIGNALS_COUNT = 1000;

int receivedSignalsCount = 0;

int waitingForConfirmation = 0;

void handleSigusr1(int signal);

void handleSigusr2(int signal);

int main(int argc, char **argv) {

    if (argc > 1) {
        SIGNALS_COUNT = atoi(argv[1]);
    }
    printf("Signals count: %d\n\n", SIGNALS_COUNT);

    char *receiversFunctionName = "receiver.o";

    if (signal(SIGUSR1, handleSigusr1) == SIG_ERR) {
        printf("\nCannot catch SIGUSR1\n");
        exit(-1);
    }
    if (signal(SIGUSR2, handleSigusr2) == SIG_ERR) {
        printf("\nCannot catch SIGUSR2\n");
        exit(-1);
    }

    int pid = fork();

    if (pid == 0) {
        assert(execl(receiversFunctionName, receiversFunctionName, NULL) > -1);
    }

    printf("Waiting for child to finish configuration...\n\n");
    usleep(1500);

    int i = 0;
    while (i < SIGNALS_COUNT) {

        if (!waitingForConfirmation) {
            kill(pid, SIGUSR1);
            printf("Sending %d. SIGUSR1 signal\n", i + 1);
            waitingForConfirmation = 1;
            i++;
        }

    }

    printf("Sending SIGUSR2 signal to child\n");

    while(waitingForConfirmation) {}

    kill(pid, SIGUSR2);

    while (1) { }

    return 0;
}

void handleSigusr1(int signal) {

    receivedSignalsCount++;
    printf("Parent got signal! \t%d\n", receivedSignalsCount);
    waitingForConfirmation = 0;
}

void handleSigusr2(int signal) {

    int status = -1;
    wait(&status);
    if (WIFEXITED(status)) {
        printf("Child terminated normally.\n\n");
    } else {
        printf("Error: child did not terminate normally.\n");
        exit(-1);
    }

    printf("Parent process received: %d signals out of %d\n\n", receivedSignalsCount, SIGNALS_COUNT);
    exit(0);
}