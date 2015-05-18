#ifndef COMMONS_H
#define COMMONS_H


#define BUFFER_SIZE 100
#define MAX_NUM 255
#define ITERATION_COUNT 10

#define PRODUCER 1
#define CONSUMER 0

#define MEMORY_NAME "/sharedMemory"

#define AVAILABLE_CELLS_SEM_NAME "/availableCellsSem"
#define SUBMITTED_JOBS_SEM_NAME "/submittedJobsSem"
#define WRITE_INDEX_SEM_NAME "/writeIndexSem"
#define READ_INDEX_SEM_NAME "/readIndexSem"


struct sharedUseSt {
    char buffer[BUFFER_SIZE];
    int writeIndex;
    int readIndex;
};

#endif //COMMONS_H
