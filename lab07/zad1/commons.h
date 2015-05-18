#ifndef COMMONS_H
#define COMMONS_H


#define BUFFER_SIZE 2048
#define PATH_FOR_KEY "."
#define NUM_OF_SEMAPHORES 4
#define MAX_NUM 255
#define ITERATION_COUNT 10

#define PRODUCER 1
#define CONSUMER 0

#define AVAILABLE_CELLS_SEM 0
#define SUBMITTED_JOBS_SEM 1
#define WRITE_INDEX_SEM 2
#define READ_INDEX_SEM 3


struct sharedUseSt {
    char buffer[BUFFER_SIZE];
    int writeIndex;
    int readIndex;
};

#endif //COMMONS_H
