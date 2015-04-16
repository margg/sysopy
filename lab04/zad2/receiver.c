#include<stdio.h>
#include<signal.h>
#include <unistd.h>
#include <stdlib.h>

int receivedSignalsCountReceiver = 0;

int parentPid;

void handleSigusr1Receiver(int signal);

void handleSigusr2Receiver(int signal);

int main(int argc, char **argv) {

    parentPid = getppid();

    if (signal(SIGUSR1, handleSigusr1Receiver) == SIG_ERR) {
        printf("\nCannot catch SIGUSR1\n");
        exit(-1);
    }
    if (signal(SIGUSR2, handleSigusr2Receiver) == SIG_ERR) {
        printf("\nCannot catch SIGUSR2\n");
        exit(-1);
    }

    printf("\n IM HERE \n\n");

    while (1) { }

    return 0;
}

void handleSigusr1Receiver(int signal) {

    receivedSignalsCountReceiver++;
    printf("\tSending back. ");
    printf("Sending back.\n");
    usleep(100);
    kill(parentPid, SIGUSR1);
}

void handleSigusr2Receiver(int signal) {

    printf("Child process received: %d signals\n", receivedSignalsCountReceiver);

    printf("\tSending SIGUSR2 signal to parent\n");
    kill(parentPid, SIGUSR2);

    exit(0);
}