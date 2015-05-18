#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <errno.h>
#include <sys/sem.h>
#include <math.h>
#include "commons.h"


void up(int semId, unsigned short semNumber);

void down(int semId, unsigned short semNumber);

int getNumber();

int checkIfPrime(int number);


/*
 * This program does not remove the created shared memory.
 *
 * In order to remove it, run ./quitter.out
 * */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("\nPlease specify a correct role of the program (producer/consumer)\n");
        exit(1);
    }

    int role;
    if (strcmp(argv[1], "producer") == 0) {
        role = PRODUCER;
        srand(time(NULL));
    } else if (strcmp(argv[1], "consumer") == 0) {
        role = CONSUMER;
    } else {
        printf("\nUnknown role.\nPlease specify a correct role of the program (producer/consumer)\n");
        exit(1);
    }


    int existed = 0;
    void *sharedMemory = (void *) 0;
    struct sharedUseSt *sharedBuffer;
    int sharedMemId;
    int i;

    key_t key = ftok(PATH_FOR_KEY, 1);
    sharedMemId = shmget(key, sizeof(struct sharedUseSt), 0666 | IPC_CREAT | IPC_EXCL);

    if (sharedMemId == -1) {
        if (errno == EEXIST) {
            existed = 1;
            sharedMemId = shmget(key, sizeof(struct sharedUseSt), 0666);
        } else {
            fprintf(stderr, "Function shmget() failed\n");
            exit(EXIT_FAILURE);
        }
    }

    sharedMemory = shmat(sharedMemId, NULL, 0);
    if (sharedMemory == NULL) {
        fprintf(stderr, "Function shmat() failed\n");
        exit(EXIT_FAILURE);
    }

    sharedBuffer = sharedMemory;

    if (!existed) {
        // initialize memory
        for (i = 0; i < BUFFER_SIZE; i++) {
            sharedBuffer->buffer[i] = -1;
        }
        sharedBuffer->writeIndex = 0;
        sharedBuffer->readIndex = 0;
    }

    key_t semKey = ftok(PATH_FOR_KEY, 2);

    int semId = semget(semKey, NUM_OF_SEMAPHORES, IPC_CREAT | IPC_EXCL | 0600);
    if (semId == -1) {
        semId = semget(semKey, NUM_OF_SEMAPHORES, 0600);
        if (semId == -1) {
            perror("Error while creating the semaphores.\n");
            exit(EXIT_FAILURE);
        }
    } else {
//        printf("===> semaphores initialization\n");
        sharedBuffer->writeIndex = 0;
        sharedBuffer->readIndex = 0;
        if (semctl(semId, AVAILABLE_CELLS_SEM, SETVAL, BUFFER_SIZE) == -1) {
            perror("Error while creating semaphore 0\n");
            exit(1);
        }
        if (semctl(semId, SUBMITTED_JOBS_SEM, SETVAL, 0) == -1) {
            perror("Error while creating semaphore 1\n");
            exit(1);
        }
        if (semctl(semId, WRITE_INDEX_SEM, SETVAL, 1) == -1) {
            perror("Error while creating semaphore 2\n");
            exit(1);
        }
        if (semctl(semId, READ_INDEX_SEM, SETVAL, 1) == -1) {
            perror("Error while creating semaphore 3\n");
            exit(1);
        }
    }


    if (role == CONSUMER) {
        for (i = 0; i < ITERATION_COUNT;) {
            usleep(700);

            down(semId, SUBMITTED_JOBS_SEM);
            down(semId, READ_INDEX_SEM);

            int number = (unsigned char) sharedBuffer->buffer[sharedBuffer->readIndex];

            if(number != -1) {
                int prime = checkIfPrime(number);

                int remainingJobs = semctl(semId, SUBMITTED_JOBS_SEM, GETVAL);

                char timeString[25];
                time_t currentTime;
                time(&currentTime);
                struct tm *timeStruct = localtime(&currentTime);
                strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", timeStruct);

                if (prime) {
                    printf("\n\t(%d %s) Checked number %d - prime. Remaining jobs: %d\n", getpid(), timeString, number,
                           remainingJobs);
                } else {
                    printf("\n\t(%d %s) Checked number %d - composite. Remaining jobs: %d\n", getpid(), timeString, number,
                           remainingJobs);
                }

                sharedBuffer->buffer[sharedBuffer->readIndex] = -1;
                i++;
            }
            sharedBuffer->readIndex = (sharedBuffer->readIndex + 1) % BUFFER_SIZE;

            up(semId, READ_INDEX_SEM);
            up(semId, AVAILABLE_CELLS_SEM);
        }

    } else {    // PRODUCER

        for (i = 0; i < ITERATION_COUNT;) {
            usleep(500);

            down(semId, AVAILABLE_CELLS_SEM);
            down(semId, WRITE_INDEX_SEM);

            int number = sharedBuffer->buffer[sharedBuffer->writeIndex];

            if(number == -1) {

                char timeString[25];
                time_t currentTime;
                time(&currentTime);
                struct tm *timeStruct = localtime(&currentTime);
                strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", timeStruct);

                number = (unsigned char) getNumber();

                sharedBuffer->buffer[sharedBuffer->writeIndex] = (unsigned char) number;

                int remainingJobs = semctl(semId, SUBMITTED_JOBS_SEM, GETVAL);
                printf("\n\t\t(%d %s) Added number: %d. Remaining jobs: %d\n", getpid(), timeString, number, remainingJobs + 1);
                i++;
            }

            sharedBuffer->writeIndex = (sharedBuffer->writeIndex + 1) % BUFFER_SIZE;

            up(semId, WRITE_INDEX_SEM);
            up(semId, SUBMITTED_JOBS_SEM);
        }

    }

    if (shmdt(sharedMemory) == -1) {
        fprintf(stderr, "Function shmdt() failed.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int checkIfPrime(int number) {

    if (number <= 2) {
        return 1;
    }
    if (number % 2 == 0) {
        return 0;
    }
    int sqrtN = (int) sqrt(number);
    int i;
    for (i = 3; i < sqrtN; i += 2) {
        if (number % 3 == 0) {
            return 0;
        }
    }
    return 1;
}

int getNumber() {
    return rand() % MAX_NUM;
}

static struct sembuf buf;

void up(int semId, unsigned short semNumber) {
    buf.sem_num = semNumber;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    if (semop(semId, &buf, 1) == -1) {
        perror("Error while moving semaphore up.\n");
        exit(1);
    }
}

void down(int semId, unsigned short semNumber) {
    buf.sem_num = semNumber;
    buf.sem_op = -1;
    buf.sem_flg = 0;

    if (semop(semId, &buf, 1) == -1) {
        printf("Error while moving semaphore down.\n");
        exit(1);
    }
}