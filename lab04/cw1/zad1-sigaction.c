#include <stdlib.h>
#include<stdio.h>
#include<signal.h>
#include <unistd.h>

int counter = 0;

int diffenerce = 1;

void handler(int signal) {

    if(signal == SIGINT) {
        printf("Received SIGINT (signal no. %d) - reversing\n", signal);
        diffenerce = -diffenerce;
    } else if (signal == SIGTERM){
        printf("Received SIGTERM (signal no. %d)\n\nBye!\n\n", signal);
        exit(1);
    }
}

int main(int argc, char **argv) {

    struct sigaction *signal = malloc(sizeof(struct sigaction));
    signal->sa_handler = &handler;

    if (sigaction(SIGINT, signal, NULL) == -1) {
        printf("\nCannot catch SIGINT\n");
        exit(-1);
    }

    if (sigaction(SIGTERM, signal, NULL) == -1) {
        printf("\nCannot catch SIGTERM\n");
        exit(-1);
    }

    while (1) {
//        for(counter = 1; counter<=10;) {
            printf("%d\n", counter);
            counter = counter + diffenerce;
            sleep(1);
//        }
    }

    return 0;
}