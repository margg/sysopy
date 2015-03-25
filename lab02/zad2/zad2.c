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

    printf("FileName \tSize \tLastModified \n\n");

    // usingOpendir(permissionsValue, dir);

    ftw(dirName, printPath, 512);


    return 0;
}


int printPath(const char *filePath, const struct stat *fileStat, int typeflag) {

    if (!filePath || !fileStat) {
        return -1;
    }

    char date[10];

    int currentFilePermissions = fileStat->st_mode & 511;
    if (permissionsValue == currentFilePermissions) {
        printf("%s    \t", filePath);
        printf("%d B \t", (int) fileStat->st_size);

        strftime(date, 10, "%d-%m-%y", gmtime(&(fileStat->st_mtime)));
        printf("%s \n", date);
    }

    return 0;
}


void usingOpendir(int permissionsValue, DIR *dir) {
    struct dirent *dp;
    struct stat fileStat;
    char date[10];

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {

            stat(dp->d_name, &fileStat);

            int currentFilePermissions = fileStat.st_mode & 511;
            if (permissionsValue == currentFilePermissions) {
                printf("%s    \t", dp->d_name);
                printf("%d B \t", (int) fileStat.st_size);

                strftime(date, 10, "%d-%m-%y", gmtime(&(fileStat.st_mtime)));
                printf("%s \n", date);
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