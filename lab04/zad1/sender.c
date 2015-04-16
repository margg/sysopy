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

int SIGNALS_COUNT = 100000;

int receivedSignalsCount = 0;

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

    int i;
    for (i = 0; i < SIGNALS_COUNT; i++) {
        kill(pid, SIGUSR1);
        printf("Sending %d. SIGUSR1 signal\n", i);
    }

    printf("Sending SIGUSR2 signal\n");

    kill(pid, SIGUSR2);

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