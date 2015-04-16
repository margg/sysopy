/*
*
* zad1
* -> wysyłanie sygnałów - kill(), raise() do siebie,
* -> definiowanie własnych - sigset, sigunset,
* -> sigprocmask()
* -> sigkill() sigterm()
*
* sygnały mogą być przechwycone albo zignorowane
* -> sygnały niezawodne - muszą być obsłużone
*
*
* nie kombinować, nie robić opóźnień - to, że one się pogubią z drugiej strony, to normalka, tak powinno być
*
* przesłać numer procesu jako argument
*
*
*
* zad2
* inaczej:
* -> sygnały czasu rzecz. - nie giną, wymagają potwierdzenia, wszystkie muszą być przechwycone
*
* parent czeka na sygnał od potomka żeby zacząć nadawanie
*
*
*
*/

#include<stdio.h>
#include<signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>

int SIGNALS_COUNT = 1000;

int receivedSignalsCount = 0;

void handleSigusr1(int signal);

void handleSigusr2(int signal);

int main(int argc, char **argv) {

    if (argc > 1) {
        SIGNALS_COUNT = atoi(argv[1]);
    }
    printf("Signals count: %d\n\n", SIGNALS_COUNT);

    char *receiversFunctionName = "receiver.o";

    struct sigaction sigAction1;
    sigAction1.sa_handler = handleSigusr1;
    sigAction1.sa_flags = 0;
    sigaddset(&(sigAction1.sa_mask), SIGRTMIN + 1);

    struct sigaction sigAction2;
    sigAction2.sa_handler = handleSigusr2;
    sigAction2.sa_flags = 0;
    sigaddset(&(sigAction2.sa_mask), SIGRTMIN);

    if (sigaction(SIGRTMIN, &sigAction1, NULL) == -1) {
        printf("\nCannot catch SIGRTMIN\n");
        exit(-1);
    }
    if (sigaction(SIGRTMIN + 1, &sigAction2, NULL) == -1) {
        printf("\nCannot catch SIGRTMIN + 1\n");
        exit(-1);
    }

    int pid = fork();

    if (pid == 0) {
        assert(execl(receiversFunctionName, receiversFunctionName, NULL) > -1);
    }

    printf("Waiting for child to finish configuration...\n\n");
    usleep(2000);

    int i;
    for (i = 0; i < SIGNALS_COUNT; i++) {
        kill(pid, SIGRTMIN);
        printf("Sending %d. SIGRTMIN signal\n", i + 1);
    }

    printf("Sending SIGRTMIN + 1 signal\n");

    kill(pid, SIGRTMIN + 1);

    while (1) { }

    return 0;
}

void handleSigusr1(int signal) {

    receivedSignalsCount++;
    printf("Parent got signal! \t%d\n", receivedSignalsCount);
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