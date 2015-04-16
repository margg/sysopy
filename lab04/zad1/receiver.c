#include<stdio.h>
#include<signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

int receivedSignalsCountReceiver = 0;

void handleSigusr1Receiver(int signal);

void handleSigusr2Receiver(int signal);

int main(int argc, char **argv) {

    if (signal(SIGUSR1, handleSigusr1Receiver) == SIG_ERR) {
        printf("\nCannot catch SIGUSR1\n");
        exit(-1);
    }
    if (signal(SIGUSR2, handleSigusr2Receiver) == SIG_ERR) {
        printf("\nCannot catch SIGUSR2\n");
        exit(-1);
    }

    while (1) { }

    return 0;
}

void handleSigusr1Receiver(int signal) {

    receivedSignalsCountReceiver++;
    printf("\tGot signal!\n");
}

void handleSigusr2Receiver(int signal) {

    printf("Child process received: %d signals\n", receivedSignalsCountReceiver);

    int parentPid = getppid();
    int i;
    for (i = 0; i < receivedSignalsCountReceiver; i++) {
        kill(parentPid, SIGUSR1);
        printf("\t\tSending %d. SIGUSR1 signal\n", i);
    }

    printf("\t\tSending SIGUSR2 signal\n");
    kill(parentPid, SIGUSR2);

    exit(0);
}