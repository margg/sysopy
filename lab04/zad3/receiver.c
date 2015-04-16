#include<stdio.h>
#include<signal.h>
#include <unistd.h>
#include <stdlib.h>

int receivedSignalsCountReceiver = 0;

void handleSigusr1Receiver(int signal);

void handleSigusr2Receiver(int signal);

int main(int argc, char **argv) {

    struct sigaction sigAction1;
    sigAction1.sa_handler = handleSigusr1Receiver;
    sigAction1.sa_flags = 0;
    sigaddset(&(sigAction1.sa_mask),SIGRTMIN+1);

    struct sigaction sigAction2;
    sigAction2.sa_handler = handleSigusr2Receiver;
    sigAction2.sa_flags = 0;
    sigaddset(&(sigAction2.sa_mask),SIGRTMIN);

    if(sigaction(SIGRTMIN, &sigAction1, NULL) == -1)  {
        printf("\nCannot catch SIGRTMIN\n");
        exit(-1);
    }
    if (signal(SIGRTMIN + 1, handleSigusr2Receiver) == SIG_ERR) {
        printf("\nCannot catch SIGRTMIN + 1\n");
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
        kill(parentPid, SIGRTMIN);
        printf("\t\tSending %d. SIGRTMIN signal\n", i);
    }

    printf("\t\tSending SIGRTMIN + 1 signal\n");
    kill(parentPid, SIGRTMIN + 1);

    exit(0);
}