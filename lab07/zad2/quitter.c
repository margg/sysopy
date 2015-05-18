#include <stdio.h>
#include <sys/stat.h>
//#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <semaphore.h>
#include <sys/sem.h>
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

    void *sharedMemory = (void *) 0;
    int sharedMemId;

    sharedMemId = shm_open(MEMORY_NAME, O_RDWR | O_CREAT | O_EXCL, 0777);

    if (sharedMemId == -1) {
        if (errno == EEXIST) {
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

    if (sem_unlink(AVAILABLE_CELLS_SEM_NAME) == -1) {
        fprintf(stderr, "Function sem_unlink() failed\n");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(SUBMITTED_JOBS_SEM_NAME) == -1) {
        fprintf(stderr, "Function sem_unlink() failed\n");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(WRITE_INDEX_SEM_NAME) == -1) {
        fprintf(stderr, "Function sem_unlink() failed\n");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(READ_INDEX_SEM_NAME) == -1) {
        fprintf(stderr, "Function sem_unlink() failed\n");
        exit(EXIT_FAILURE);
    }



    if (munmap(sharedMemory, sizeof(struct sharedUseSt)) == (long) MAP_FAILED) {
        printf("Function mmap() failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }


    if (shm_unlink(MEMORY_NAME) == -1) {
        fprintf(stderr, "Function shm_unlink() failed.\n");
        exit(EXIT_FAILURE);
    }

    if (close(sharedMemId) == -1) {
        fprintf(stderr, "Function close() failed\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}