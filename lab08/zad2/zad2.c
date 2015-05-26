#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define TEXT_LENGTH 1020

#define VERSION1
//#define VERSION2
//#define VERSION3

struct record {
    int id;
    char text[TEXT_LENGTH];
};

int recordsCount;
int threadsCount;

int file;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int threadsCreationFinished = 0;


int readBytes(int file, size_t bytesToRead, char *buffer);

size_t readRecords(int file, int recordsCount, struct record *records);

void *threadFind(void *textToFind);

void setCancellation(int mode);

void sigHandler(int signo) ;

int main(int argc, char const *argv[]) {

    if (argc != 5) {
        printf("\nInvalid arguments.\n\nUsage:\n\t./zad1_[1|2|3].out\t<num of threads> <file name> <records per thread> <word to find>\n\n");
        exit(EXIT_FAILURE);
    }

    threadsCount = atoi(argv[1]);
    char const *fileName = argv[2];
    recordsCount = atoi(argv[3]);
    char textToFind[strlen(argv[4])];
    strcpy(textToFind, argv[4]);

    struct sigaction sigact;
    sigact.sa_handler = sigHandler;
    sigact.sa_flags = 0;
    if(sigaction(SIGUSR1, &sigact, NULL) || sigaction(SIGTERM, &sigact, NULL)) {
        perror("Error while setting up singal handling.\n");
        exit(EXIT_FAILURE);
    }


    if ((file = open(fileName, O_RDONLY)) == -1) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    threads = malloc(threadsCount * sizeof(pthread_t));

    int i;
    for (i = 0; i < threadsCount; i++) {
        if (pthread_create(&threads[i], NULL, threadFind, textToFind) != 0) {
            perror("Error while creating a thread.");
            exit(EXIT_FAILURE);
        }
    }

    threadsCreationFinished = 1;



//    pthread_kill(threads[0], SIGUSR1);
//    pthread_kill(threads[0], SIGTERM);
//    pthread_kill(threads[0], SIGKILL);
//    pthread_kill(threads[0], SIGSTOP);

    pause();


    for (i = 0; i < threadsCount; ++i) {
        if ((pthread_join(threads[i], NULL)) != 0) {
            perror("Error while joining threads.");
            exit(EXIT_FAILURE);
        }
    }

    close(file);
    free(threads);
    return 0;
}


void sigHandler(int signo) {
    printf("Signal: %d, PID: %u, TID: %lu\n", (unsigned int)signo, getpid(), pthread_self());
}

void sigThreadHandler(int signo) {
    printf("\t Thread got signal: %d, PID: %u, TID: %lu\n", (unsigned int)signo, getpid(), pthread_self());
}


void *threadFind(void *textToFind) {
    struct record records[recordsCount];
    size_t recordsRead = 1;

#ifdef VERSION1
    if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0) {
        perror("Error while setting thread cancel type.");
        exit(EXIT_FAILURE);
    }
#endif
#ifdef VERSION3
    if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) {
        perror("Error while setting thread cancel state.");
        exit(EXIT_FAILURE);
    }
#endif

    struct sigaction sigact;
    sigact.sa_handler = sigThreadHandler;
    sigact.sa_flags = 0;
    if(sigaction(SIGUSR1, &sigact, NULL) || sigaction(SIGTERM, &sigact, NULL)) {
        perror("Error while setting up singal handling.\n");
        exit(EXIT_FAILURE);
    }

    while (!threadsCreationFinished) { }

    int readingIteration = 0;

    while (recordsRead > 0) {

        readingIteration++;

#ifdef VERSION2
        pthread_testcancel();
        setCancellation(PTHREAD_CANCEL_DISABLE);
#endif

        pthread_mutex_lock(&mutex);
        recordsRead = readRecords(file, recordsCount, records);
        pthread_mutex_unlock(&mutex);

        int i;
        for (i = 0; i < recordsRead; i++) {
            printf("TID: %lu, RecordID: %d, \treading iteration: %d\n", (unsigned long) pthread_self(), records[i].id, readingIteration);

            char *substring = strstr(records[i].text, textToFind);

            if (substring) {
                printf("\tTID: %lu, text found in record: %d\n", (unsigned long) pthread_self(), records[i].id);
#ifndef VERSION3
                int j;
                for (j = 0; j < threadsCount; j++) {
                    if (!pthread_equal(threads[j], pthread_self())) {
                        pthread_cancel(threads[j]);
                    }
                }
                return 0;
#endif
            }
            usleep(20);   // for visible performance only
        }

#ifdef VERSION2
        setCancellation(PTHREAD_CANCEL_ENABLE);
#endif
    }
    return 0;
}

void setCancellation(int mode) {
    if (pthread_setcancelstate(mode, NULL) != 0) {
        perror("Error while setting thread cancel state.");
        exit(EXIT_FAILURE);
    }
}

size_t readRecords(int file, int recordsCount, struct record *records) {
    size_t recordSize = sizeof(struct record);
    int bytesRead = readBytes(file, recordSize * recordsCount, (char *) records);
    return bytesRead / recordSize;
}

int readBytes(int file, size_t bytesToRead, char *buffer) {
    int bytesRead = 0;
    while (bytesToRead > bytesRead) {
        ssize_t actual = read(file, buffer, bytesToRead);
        if (actual == 0) {
            return bytesRead;
        }
        bytesRead += actual;
    }
    return bytesRead;
}