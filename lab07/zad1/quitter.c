#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/sem.h>
#include "commons.h"


int main(int argc, char *argv[]) {

    void *sharedMemory = (void *) 0;
    int sharedMemId;

    key_t key = ftok(PATH_FOR_KEY, 1);
    sharedMemId = shmget(key, sizeof(struct sharedUseSt), 0666 | IPC_CREAT | IPC_EXCL);

    if (sharedMemId == -1) {
        if (errno == EEXIST) {
            sharedMemId = shmget(key, sizeof(struct sharedUseSt), 0666);
        } else {
            fprintf(stderr, "Function shmget() failed\n");
            exit(EXIT_FAILURE);
        }
    }

    sharedMemory = shmat(sharedMemId, NULL, 0);
    if (sharedMemory == NULL) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    key_t semKey = ftok(PATH_FOR_KEY, 2);

    int semId = semget(semKey, NUM_OF_SEMAPHORES, IPC_CREAT | IPC_EXCL | 0600);
    if (semId == -1) {
        semId = semget(semKey, NUM_OF_SEMAPHORES, 0600);
        if (semId == -1) {
            perror("Error while creating the semaphore table.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (semctl(semId, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Function semctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
    if (shmdt(sharedMemory) == -1) {
        fprintf(stderr, "Function shmdt() failed\n");
        exit(EXIT_FAILURE);
    }
    if (shmctl(sharedMemId, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Function shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}