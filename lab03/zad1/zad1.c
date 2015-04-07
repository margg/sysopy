#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <stdlib.h>
#include <assert.h>
#include <sched.h>
#include <time.h>

#define STACK_SIZE 1000
#define FORKS_COUNT 10000

int counter = 0;
int forksCount = FORKS_COUNT;

clock_t startClk;
clock_t endClk;

int realTimeChildren = 0;
struct tms startTimes;
struct tms endTimes;


int childFunction(void *args);

void startClock();

void endClock();

int childFunction(void *args) {
    clock_t real = clock();
    counter = counter + 1;
//    printf("Counter: %d\n", counter);
    _exit(clock() - real);
}


int main(int argc, char **argv) {


    if (argc < 2) {
        printf("Using default value of amount of forks (%d).\n", FORKS_COUNT);
        printf("If you want to specify the amount, use it as an argument:\n");
        printf("\n\t <prog name> <number of forksCount>\n\n");
    } else {
        forksCount = atoi(argv[1]);
    }

    if (forksCount < 1) {
        printf("Incorrect number of forks. Please specify a positive number.\n");
        return 1;
    }

    char *childStack = malloc(STACK_SIZE);
    assert(childStack);
    int status;
    int i;
    __pid_t pid;


#ifdef FORK
    printf("Fork method: FORK\n\n");
#endif
#ifdef VFORK
    printf("Fork method: VFORK\n\n");
#endif
#ifdef CLONE
    printf("Fork method: CLONE\n\n");
#endif
#ifdef VCLONE
    printf("Fork method: VCLONE\n\n");
#endif


    startClock();

    for (i = 0; i < forksCount; ++i) {

#ifdef FORK
        pid = fork();
        if (pid >= 0) {
            if (pid == 0) { // child process
                childFunction(NULL);
            } else
#else
#ifdef VFORK
        pid = vfork();
        if (pid >= 0) {
            if (pid == 0) { // child process
                childFunction(NULL);
            } else
#else
#ifdef CLONE
        pid = clone(childFunction, childStack + STACK_SIZE, SIGCHLD, NULL);
        if (pid >= 0) {
#else
#ifdef VCLONE
        pid = clone(childFunction, childStack + STACK_SIZE, SIGCHLD | CLONE_VM | CLONE_VFORK, NULL);
        if (pid >= 0) {
#else
        printf("No fork method specified. Using default - CLONE (as fork).\n");
        pid = clone(childFunction, childStack + STACK_SIZE, SIGCHLD, NULL);
        if (pid >= 0) {
#endif
#endif
#endif
#endif

            { // parent process
                waitpid(-1, &status, 0);
                if (WIFEXITED(status)) {
                    realTimeChildren += WEXITSTATUS(status);
                } else {
                    printf("Error: child did not terminate normally.\n");
                    exit(-1);
                }
            }

        } else {
            printf("\n Fork failed, quitting.\n");
            return -1;
        }

    }

    endClock();

    printf("\nFinal counter value: %d\n\n", counter);

    return 0;
}

void startClock() {
    startClk = times(&startTimes);
}

void endClock() {
    endClk = times(&endTimes);
    double realChildren = ((double) (realTimeChildren)) / CLOCKS_PER_SEC;
    double userChildren = ((double) (endTimes.tms_cutime - startTimes.tms_cutime)) / sysconf(_SC_CLK_TCK);
    double systemChildren = ((double) (endTimes.tms_cstime - startTimes.tms_cstime)) / sysconf(_SC_CLK_TCK);
    double realParent = ((double) (endClk - startClk)) / sysconf(_SC_CLK_TCK);
    double userParent = ((double) (endTimes.tms_utime - startTimes.tms_utime)) / sysconf(_SC_CLK_TCK);
    double systemParent = ((double) (endTimes.tms_stime - startTimes.tms_stime)) / sysconf(_SC_CLK_TCK);
    printf("Children count\t%d\n", forksCount);
    printf("\tReal Sum  \t%.6f", realChildren + realParent);
    printf("\tUser Sum  \t%.6f", userChildren + userParent);
    printf("\tSystem Sum  \t%.6f", systemChildren + systemParent);
    printf("\tSystem+User Sum  \t%.6f\n", userChildren + userParent + systemChildren + systemParent);
    printf("\tReal Child\t%.6f", realChildren);
    printf("\tUser Child\t%.6f", userChildren);
    printf("\tSystem Child\t%.6f", systemChildren);
    printf("\tSystem+User Child\t%.6f\n", userChildren + systemChildren);
    printf("\tReal Parent\t%.6f", realParent);
    printf("\tUser Parent\t%.6f", userParent);
    printf("\tSystem Parent\t%.6f", systemParent);
    printf("\tSystem+User Parent\t%.6f \n", userParent + systemParent);

    char*fileName = malloc(25 * sizeof(char));

#ifdef FORK
    fileName = "results_fork.csv";
#endif
#ifdef VFORK
    fileName = "results_vfork.csv";
#endif
#ifdef CLONE
    fileName = "results_clone.csv";
#endif
#ifdef VCLONE
    fileName = "results_vclone.csv";
#endif

    FILE *fileDes = fopen(fileName, "a+");
    fprintf(fileDes, "%d;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f;\n", forksCount,
            realChildren + realParent, userChildren + userParent, systemChildren + systemParent, userChildren + userParent + systemChildren + systemParent,
            realChildren, userChildren, systemChildren, userChildren + systemChildren,
            realParent, userParent, systemParent, userParent + systemParent);
}




