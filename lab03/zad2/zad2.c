#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <unistd.h>
#include <wait.h>

int w = 0;
int showFileData = 0;
char *path;

void countFiles(char **argv);

char **createChildArgList(char *programName, int w, int showFileData, char *dirPath);

int main(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp("-w", argv[i]) == 0) {
            w = 1;
        } else if (strcmp("--show-data", argv[i]) == 0) {
            showFileData = 1;
        } else if (strcmp("--help", argv[i]) == 0) {
            printf("\nAvailable options:\n\n");
            printf("\t-w\t\tsleep for 20 s after reading directory\n");
            printf("\t--show-data \tshow information about each file and directory\n\n");
            exit(0);
        } else {
            path = argv[i];
        }
    }
    countFiles(argv);
    return 0;
}


void countFiles(char **argv) {
    int sumOfFiles = 0;
    int processCount = 0;

    char *pathToBrowse = malloc(256 * sizeof(char));

    if (realpath(path, pathToBrowse) == NULL) {
        printf("Running for current directory.\n\n");
        realpath(".", pathToBrowse);
    }

    DIR *dir = opendir(pathToBrowse);
    assert(dir != NULL);
    struct dirent *dirEntry;
    char dirPath[256];
    char filePath[256];
    char **childArgs;
    int pid;

    while ((dirEntry = readdir(dir))) {
        if (dirEntry->d_type == DT_DIR) {
            if (strcmp(".", dirEntry->d_name) != 0 && strcmp("..", dirEntry->d_name) != 0) {
                sprintf(dirPath, "%s/%s", pathToBrowse, dirEntry->d_name);
                processCount++;
                pid = fork();
                if (pid == 0) {
                    childArgs = createChildArgList(argv[0], w, showFileData, dirPath);

                    assert(execv(argv[0], childArgs) > -1);
                }
            }
        } else {
            sprintf(filePath, "%s/%s", pathToBrowse, dirEntry->d_name);
            sumOfFiles++;

            if (showFileData) {
                printf("\t\tFile: %s\n", filePath);
            }
        }
    }

    if (w) {
        sleep(20);
    }

    assert(closedir(dir) > -1);

    int filesInDir = sumOfFiles;
    int status;

    while (processCount > 0) {
        status = -1;
        wait(&status);
        if (WIFEXITED(status)) {
            sumOfFiles += WEXITSTATUS(status);
            processCount--;
        } else {
            printf("Error: child did not terminate normally.\n");
            exit(-1);
        }
    }

    if (showFileData) {
        printf("\n\tDirectory name: %s\t    Regular files in the directory:  %d\t    Sum of all regular files:  %d\n\n", pathToBrowse, filesInDir, sumOfFiles);
    }

    _exit(sumOfFiles);
}

char **createChildArgList(char *programName, int w, int showFileData, char *dirPath) {

    int size = 3 + w + showFileData;
    char **childArgs = malloc(size * sizeof(char *));

    childArgs[0] = programName;
    childArgs[1] = dirPath;
    int argOffset = 2;
    if (w) {
        childArgs[argOffset] = "-w";
        argOffset++;
    }
    if (showFileData) {
        childArgs[argOffset] = "--show-data";
    }
    return childArgs;
}