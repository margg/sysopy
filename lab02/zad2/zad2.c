#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <ftw.h>


int permissionsValue;

int printPath(const char *filePath, const struct stat *fileStat, int typeflag);

int countPermissionsValue(char *permissions);

void exitWrongArguments();

void usingOpendir(int permissionsValue, DIR *dir);

int main(int argc, char **argv) {

    if (argc != 3) {
        exitWrongArguments();
    }

    char dirName[100];

    realpath(argv[1], dirName);

    char *permissions = argv[2];
    permissionsValue = countPermissionsValue(permissions);

    if (permissionsValue == -1) {
        exitWrongArguments();
    }

    DIR *dir;

    if ((dir = opendir(dirName)) == NULL) {
        printf("Error while opening directory %s.", dirName);
        exit(1);
    }

    printf("FileName \t\tSize \t\tLastModified \n\n");

#ifndef USEFTW
    usingOpendir(permissionsValue, dir);
#else
    ftw(dirName, printPath, 512);
#endif

    printf("\n\n");

    return 0;
}


int printPath(const char *filePath, const struct stat *fileStat, int typeflag) {

    if (!filePath || !fileStat) {
        return -1;
    }
    int currentFilePermissions = fileStat->st_mode & 511;

    if (S_ISREG(fileStat->st_mode) && permissionsValue == currentFilePermissions) {
        printf("%-60s %10d B \t%s", filePath, (int) fileStat->st_size, ctime(&(fileStat->st_mtime)));
    }

    return 0;
}


void usingOpendir(int permissionsValue, DIR *dir) {
    struct dirent *dp;
    struct stat fileStat;

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {

            stat(dp->d_name, &fileStat);

            int currentFilePermissions = fileStat.st_mode & 511;
            if (permissionsValue == currentFilePermissions) {
                printf("%-20s %10d B \t%s", dp->d_name, (int) fileStat.st_size, ctime(&(fileStat.st_mtime)));
            }
        }
    }

    closedir(dir);
}

void exitWrongArguments() {
    printf("Wrong arguments. Usage:\n\n");
    printf("\t <path/to/directory> <permissions>\n");
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