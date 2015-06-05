#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define PHILOSOPHERS_COUNT 5

int philosophers[PHILOSOPHERS_COUNT];
int eatingPhilosophersCount = 0;
pthread_mutex_t conductor = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t forks[PHILOSOPHERS_COUNT];
pthread_cond_t availableSeat = PTHREAD_COND_INITIALIZER;

pthread_t philosopherThreads[PHILOSOPHERS_COUNT];
int isEating[PHILOSOPHERS_COUNT];
int forkUser[PHILOSOPHERS_COUNT];

void *eat(void *philosopherId);

void sigHandler(int signo);


int main(int argc, char **argv) {

    struct sigaction sigact;
    sigact.sa_handler = sigHandler;
    sigact.sa_flags = 0;
    if (sigaction(SIGINT, &sigact, NULL) || sigaction(SIGTERM, &sigact, NULL)) {
        perror("Error while setting up singal handling.\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);

    int i;
    for (i = 0; i < PHILOSOPHERS_COUNT; i++) {
        philosophers[i] = i;
        isEating[i] = 0;
        forkUser[i] = -1;
        pthread_mutex_init(&forks[i], &attr);
    }

    for (i = 0; i < PHILOSOPHERS_COUNT; i++) {
        pthread_create(&philosopherThreads[i], NULL, eat, &philosophers[i]);
    }

    while (1) { }

    return 0;
}

void acquire(int philosopher, int fork) {
    pthread_mutex_lock(&forks[fork]);
    forkUser[fork] = philosopher;
}

void release(int fork) {
    forkUser[fork] = -1;
    pthread_mutex_unlock(&forks[fork]);
}

void *eat(void *philosopherId) {
    int id = *((int *) philosopherId);

    srand(time(NULL));

    int leftFork = id;
    int rightFork = (id + 1) % PHILOSOPHERS_COUNT;

    while (1) {
        usleep(rand() % 1000);

        pthread_mutex_lock(&conductor);
        while (eatingPhilosophersCount > PHILOSOPHERS_COUNT - 2) {
            pthread_cond_wait(&availableSeat, &conductor);
        }
        eatingPhilosophersCount++;
        pthread_mutex_unlock(&conductor);

        acquire(id, leftFork);
        acquire(id, rightFork);

        isEating[id] = 1;

        printf("Philosopher %d STARTED eating with forks (%d, %d)\n", id, leftFork, rightFork);

        usleep(rand() % 500);
        isEating[id] = 0;

        release(rightFork);
        release(leftFork);
        printf("\tPhilosopher %d FINISHED eating.\n", id);

        pthread_mutex_lock(&conductor);
        eatingPhilosophersCount--;
        if (eatingPhilosophersCount < PHILOSOPHERS_COUNT - 1) {
            pthread_cond_signal(&availableSeat);
        }
        pthread_mutex_unlock(&conductor);
    }

    return NULL;
}

void sigHandler(int signo) {
    printf("\nCancelling threads...\n");
    int i;
    for (i = 0; i < PHILOSOPHERS_COUNT; i++) {
        pthread_cancel(philosopherThreads[i]);
    }
    printf("Finished.\n");
    exit(EXIT_SUCCESS);
}