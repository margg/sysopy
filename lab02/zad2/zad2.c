#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <ftw.h>


int permissionsValue;

int printFiles(const char *filePath, const struct stat *fileStat, int typeflag);

int countPermissionsValue(char *permissions);

void exitWrongArguments();

void printUsingOpendir(int permissionsValue, char *dirPath);

char *validatePath(char *dirPath);

int main(int argc, char **argv) {

    if (argc != 3) {
        exitWrongArguments();
    }

    char *dirPath = malloc(256 * sizeof(char));

    realpath(argv[1], dirPath);
    dirPath = validatePath(dirPath);

    char *permissions = argv[2];
    permissionsValue = countPermissionsValue(permissions);

    if (permissionsValue == -1) {
        exitWrongArguments();
    }

    printf("FileName \t\tSize \t\tLastModified \n\n");

#ifndef USEFTW
    printUsingOpendir(permissionsValue, dirPath);
#else
    ftw(dirPath, printFiles, 512);
#endif

    printf("\n\n");

    free(dirPath);

    return 0;
}

int printFiles(const char *filePath, const struct stat *fileStat, int typeflag) {

    if (!filePath || !fileStat) {
        return -1;
    }
    int currentFilePermissions = fileStat->st_mode & 511;

    if (S_ISREG(fileStat->st_mode) && permissionsValue == currentFilePermissions) {
        printf("%-60s %10d B \t%s", filePath, (int) fileStat->st_size, ctime(&(fileStat->st_mtime)));
    }

    return 0;
}


void printUsingOpendir(int permissionsValue, char *dirPath) {

    DIR *dir;

    if ((dir = opendir(dirPath)) == NULL) {
        printf("Error while opening directory %s.", dirPath);
        exit(1);
    }

    struct dirent *dirEntry;
    struct stat fileStat;
    int currentFilePermissions;

    while ((dirEntry = readdir(dir)) != NULL) {

        char subDirPath[256];

        if (dirEntry->d_type == DT_DIR) {
            if (strcmp(dirEntry->d_name, "..") && strcmp(dirEntry->d_name, ".")) {
                strcpy(subDirPath, dirPath);
                strcat(subDirPath, dirEntry->d_name);
                strcat(subDirPath, "/");

                printUsingOpendir(permissionsValue, subDirPath);
            }
        } else {
            strcpy(subDirPath, dirPath);
            strcat(subDirPath, dirEntry->d_name);

            if (lstat(subDirPath, &fileStat) < 0) {
                printf("Error while getting file stats.\n");
                exit(1);
            }

            currentFilePermissions = fileStat.st_mode & 511;

            if (permissionsValue == currentFilePermissions) {
                printf("%-20s %10d B \t%s", dirEntry->d_name, (int) fileStat.st_size, ctime(&(fileStat.st_mtime)));
            }
        }
    }

    closedir(dir);
}

char *validatePath(char *dirPath) {
    size_t length = strlen(dirPath);
    if (dirPath[length - 1] != '/') {
        char *newDirPath = malloc((length + 2) * sizeof(char));
        strcpy(newDirPath, dirPath);
        newDirPath[length] = '/';
        newDirPath[length + 1] = '\0';
        dirPath = newDirPath;
    }
    return dirPath;
}

void exitWrongArguments() {
    printf("Wrong arguments. Usage:\n");
    printf("\t<program name> <path/to/directory> <permissions>\n");
    printf("where <permissions> should be of type: rwxrwxrwx, e.g. rw-r-xr--\n");
    exit(1);
}

int countPermissionsValue(char *permissions) {
    int permissionsSum = 0;
    if (strlen(permissions) != 9) {
        return -1;
    } else {
        int i;
        int weight = 256;
        for (i = 0; i < 9;) {
            if (permissions[i] == 'r') {
                permissionsSum = permissionsSum + weight;
            }
            weight = weight / 2;
            i++;
            if (permissions[i] == 'w') {
                permissionsSum = permissionsSum + weight;
            }
            weight = weight / 2;
            i++;
            if (permissions[i] == 'x') {
                permissionsSum = permissionsSum + weight;
            }
            weight = weight / 2;
            i++;
        }
    }
    return permissionsSum;
}