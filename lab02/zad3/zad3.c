#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "generator.h"

int showLocked(int fileDescriptor, short *locks);

int setLock(int fileDescriptor, short byteNumber, short type, short *locks);

void readFromFile(int fd, int byte);

int writeToFile(int fd, int byte, char c);

int setLock(int fileDescriptor, short byteNumber, short type, short *locks) {
    struct flock *fl = (struct flock *) malloc(sizeof(struct flock));
    fl->l_type = type;
    fl->l_whence = SEEK_SET;
    fl->l_start = byteNumber;
    fl->l_len = 1;

    if (fcntl(fileDescriptor, F_SETLK, fl) == -1) {
        printf("Could not create the lock. (Descriptor: %d, byteNumber: %d).\n", fileDescriptor, byteNumber);
        return -1;
    }

    locks[byteNumber] = type;

    if (type == F_UNLCK) {
        printf("Unlocked %d. character.\n", byteNumber);
    } else if (type == F_RDLCK) {
        printf("Read lock on %d. character.\n", byteNumber);
    } else {
        printf("Write and read lock on %d. character.\n", byteNumber);
    }

    return 1;
}

int showLocked(int fileDescriptor, short *locks) {
    int byteNumber;
    int length = (int) lseek(fileDescriptor, 0, SEEK_END);
    struct flock fl;
    fl.l_whence = SEEK_SET;
    fl.l_len = 1;

    for (byteNumber = 0; byteNumber < length; byteNumber++) {
        fl.l_type = F_WRLCK;
        fl.l_start = byteNumber;
        if (fcntl(fileDescriptor, F_GETLK, &fl) == -1) {
            printf("Error while getting the lock at position %d.\n", byteNumber);
            exit(-2);
        }

        if (fl.l_type != F_UNLCK) {
            if (fl.l_type == F_RDLCK) {
                printf("=> Read lock on character %d; PID: %d\n", byteNumber, fl.l_pid);
            } else if (fl.l_type == F_WRLCK) {
                printf("=> Write lock on character %d; PID: %d\n", byteNumber, fl.l_pid);
            }
        }
    }

    return 0;
}

void readFromFile(int fd, int byte) {
    struct flock fl;
    fl.l_type = F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = byte;
    fl.l_len = 1;

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        printf("Error while reading from file.\n");
        return;
    }

    lseek(fd, byte, SEEK_SET);
    char c;
    read(fd, &c, sizeof(char));
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    printf("%c\n", c);
}

int writeToFile(int fd, int byte, char c) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = byte;
    fl.l_len = 1;

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        printf("Error while writing to file.\n");
        return -1;
    }

    lseek(fd, byte, SEEK_SET);
    write(fd, &c, sizeof(char));
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    return 0;
}

int main(int argc, char **argv) {
    char *fileName;
    int fileDescriptor;

    if (argc < 2) {
        fileName = argv[1];
    }


    fileName = "genFile";

    generateRecordsFile(fileName, 5, 10);

    if ((fileDescriptor = open(fileName, O_RDWR)) == -1) {
        printf("Cannot open file %s.", fileName);
        return -2;
    }

    int length = (int) lseek(fileDescriptor, 0, SEEK_END);
    short *locks = malloc(length * sizeof(short));

    short byteNumber;
    for (byteNumber = 0; byteNumber < length; byteNumber++) {
        locks[byteNumber] = F_UNLCK;
    }

    char *type = malloc(sizeof(char) * 64);
    char *cmd = (char *) malloc(sizeof(char) * 64);
    char c;
    printf("Type help to see the list of possible actions.\n");

    while (strcmp(cmd, "bye") != 0) {
        scanf("%s", cmd);

        if (strcmp(cmd, "help") == 0) {
            printf("usage:\n");
            printf("\tlock <position> [r|w|u]\n");
            printf("\t\t- read lock(r), read and write lock(w), unlock(u) character at <position>\n");
            printf("\tdisplay - show available locks\n");
            printf("\tread <position> - read character at <position>\n");
            printf("\twrite position char - write <char> at <position>\n");
            printf("\texit - exit program\n");

//            printf("rygiel do odczytu pozwala na odczyt a zabrania zapisu\n");
//            printf("rygiel do zapisu nie pozwala ani na odczyt ani na zapis\n");

        } else if (strcmp(cmd, "lock") == 0) {
            scanf("%d", &byteNumber);
            scanf("%s", type);

            if (strcmp(type, "r") == 0) {
                setLock(fileDescriptor, byteNumber, F_RDLCK, locks);
            } else if (strcmp(type, "w") == 0) {
                setLock(fileDescriptor, byteNumber, F_WRLCK, locks);
            } else if (strcmp(type, "u") == 0) {
                setLock(fileDescriptor, byteNumber, F_UNLCK, locks);
            } else {
                printf("Wrong option. Type help to see the list of possible actions.\n");
            }
        } else if (strcmp(cmd, "display") == 0) {
            showLocked(fileDescriptor, locks);
            printf("\nEND\n");
        } else if (strcmp(cmd, "read") == 0) {
            scanf("%d", &byteNumber);
            readFromFile(fileDescriptor, byteNumber);
        } else if (strcmp(cmd, "write") == 0) {
            scanf("%d", &byteNumber);
            scanf(" %c", &c);
            writeToFile(fileDescriptor, byteNumber, c);
        } else if (strcmp(cmd, "exit") == 0) {
            return 0;
        } else {
            printf("Wrong option. Type help to see the list of possible actions.\n");
        }
    }

    close(fileDescriptor);
    free(locks);
    free(type);
    free(cmd);

    return 0;
}