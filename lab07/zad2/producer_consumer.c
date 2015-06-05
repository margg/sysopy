#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <semaphore.h>
#include "commons.h"




void up(sem_t *semaphore);

void down(sem_t *semaphore);

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

    sharedMemId = shm_open(MEMORY_NAME, O_RDWR | O_CREAT | O_EXCL, 0777);

    if (sharedMemId == -1) {
        if (errno == EEXIST) {
            existed = 1;
            sharedMemId = shm_open(MEMORY_NAME, O_RDWR, 0777);
        } else {
            fprintf(stderr, "Function shm_open() failed\n");
            exit(EXIT_FAILURE);
        }
    }

    if (ftruncate(sharedMemId, sizeof(struct sharedUseSt)) == -1) {
        perror("Function ftruncate() failed.\n");
        exit(EXIT_FAILURE);
    }

    sharedMemory = mmap(0, sizeof(struct sharedUseSt), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemId, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("Function mmap() failed.\n");
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

    sem_t *availableSem = sem_open(AVAILABLE_CELLS_SEM_NAME,  O_RDWR | O_CREAT | O_EXCL, 0600, BUFFER_SIZE);
    if (availableSem == NULL) {
        availableSem = sem_open(AVAILABLE_CELLS_SEM_NAME, O_RDWR);
        if (availableSem == NULL) {
            perror("Error while creating the semaphores.\n");
            exit(EXIT_FAILURE);
        }
    }
    sem_t *submittedJobsSem = sem_open(SUBMITTED_JOBS_SEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0600, 0);
    if (submittedJobsSem == NULL) {
        submittedJobsSem = sem_open(SUBMITTED_JOBS_SEM_NAME, O_RDWR);
        if (submittedJobsSem == NULL) {
            perror("Error while creating the semaphores.\n");
            exit(EXIT_FAILURE);
        }
    }
    sem_t *writeIndexSem = sem_open(WRITE_INDEX_SEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0600, 1);
    if (writeIndexSem == NULL) {
        writeIndexSem = sem_open(WRITE_INDEX_SEM_NAME, O_RDWR);
        if (writeIndexSem == NULL) {
            perror("Error while creating the semaphores.\n");
            exit(EXIT_FAILURE);
        }
    }
    sem_t *readIndexSem = sem_open(READ_INDEX_SEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0600, 1);
    if (readIndexSem == NULL) {
        readIndexSem = sem_open(READ_INDEX_SEM_NAME, O_RDWR);
        if (readIndexSem == NULL) {
            perror("Error while creating the semaphores.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (role == CONSUMER) {
        for (i = 0; i < ITERATION_COUNT;) {
            usleep(70000);

            down(submittedJobsSem);
            down(readIndexSem);

            int number = (unsigned char) sharedBuffer->buffer[sharedBuffer->readIndex];

            if (number != -1) {
                int prime = checkIfPrime(number);

                int remainingJobs;
                sem_getvalue(submittedJobsSem, &remainingJobs);

                char timeString[25];
                time_t currentTime;
                time(&currentTime);
                struct tm *timeStruct = localtime(&currentTime);
                strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", timeStruct);

                if (prime) {
                    printf("\n\t(%d %s) Checked number %d - prime. Remaining jobs: %d\n",
                           getpid(), timeString, number, remainingJobs);
                } else {
                    printf("\n\t(%d %s) Checked number %d - composite. Remaining jobs: %d\n",
                           getpid(), timeString, number, remainingJobs);
                }

                sharedBuffer->buffer[sharedBuffer->readIndex] = -1;
                i++;
            }
            sharedBuffer->readIndex = (sharedBuffer->readIndex + 1) % BUFFER_SIZE;

            up(readIndexSem);
            up(availableSem);
        }

    } else {    // PRODUCER

        for (i = 0; i < ITERATION_COUNT;) {
            usleep(50000);

            down(availableSem);
            down(writeIndexSem);

            int number = sharedBuffer->buffer[sharedBuffer->writeIndex];

            if (number == -1) {

                char timeString[25];
                time_t currentTime;
                time(&currentTime);
                struct tm *timeStruct = localtime(&currentTime);
                strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", timeStruct);

                number = (unsigned char) getNumber();

                sharedBuffer->buffer[sharedBuffer->writeIndex] = (unsigned char) number;

                int remainingJobs;
                sem_getvalue(submittedJobsSem, &remainingJobs);

                printf("\n\t\t(%d %s) Added number: %d. Remaining jobs: %d\n",
                       getpid(), timeString, number, remainingJobs + 1);
                i++;
            }

            sharedBuffer->writeIndex = (sharedBuffer->writeIndex + 1) % BUFFER_SIZE;

            up(writeIndexSem);
            up(submittedJobsSem);
        }
    }


    if (sem_close(availableSem) == -1) {
        fprintf(stderr, "Function sem_close() failed\n");
        exit(EXIT_FAILURE);
    }
    if (sem_close(submittedJobsSem) == -1) {
        fprintf(stderr, "Function sem_close() failed\n");
        exit(EXIT_FAILURE);
    }
    if (sem_close(writeIndexSem) == -1) {
        fprintf(stderr, "Function sem_close() failed\n");
        exit(EXIT_FAILURE);
    }
    if (sem_close(readIndexSem) == -1) {
        fprintf(stderr, "Function sem_close() failed\n");
        exit(EXIT_FAILURE);
    }


    if (munmap(sharedMemory, sizeof(struct sharedUseSt)) == (long) MAP_FAILED) {
        printf("Function mmap() failed: %s\n", strerror(errno));
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

void up(sem_t *semaphore) {
    if (sem_post(semaphore) == -1) {
        perror("Error while moving semaphore up.\n");
        exit(EXIT_FAILURE);
    }
}

void down(sem_t *semaphore) {
    if (sem_wait(semaphore) == -1) {
        printf("Error while moving semaphore down.\n");
        exit(EXIT_FAILURE);
    }
}