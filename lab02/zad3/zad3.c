#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int showLocked(int fileDescriptor);

int setLock(int fileDescriptor, short offset, short type);

void readFromFile(int fd, short offset);

int writeToFile(int fd, short offset, char character);

int setLock(int fileDescriptor, short offset, short type) {
    struct flock lock;

    lock.l_type = type;
    lock.l_whence = SEEK_SET;
    lock.l_start = offset;
    lock.l_len = 1;

    if (fcntl(fileDescriptor, F_SETLK, &lock) == -1) {
        printf("Could not create the lock. (Descriptor: %d, offset: %d).\n", fileDescriptor, offset);
        return -1;
    }
    
    return offset;
}

int showLocked(int fileDescriptor) {

    int offset;
    int length = (int) lseek(fileDescriptor, 0, SEEK_END);

    if (length == -1) {
        printf("Error while seeking the offset.\n");
        return -1;
    }

    struct flock fl;

    for (offset = 0; offset < length; offset++) {
        fl.l_whence = SEEK_SET;
        fl.l_len = 1;
        fl.l_type = F_WRLCK;
        fl.l_start = offset;
        if (fcntl(fileDescriptor, F_GETLK, &fl) == -1) {
            printf("Error while getting the lock at position %d.\n", offset);
            exit(-2);
        }

        if (fl.l_type == F_RDLCK) {
            printf("=> Read lock on character %d; PID: %d\n", offset, fl.l_pid);
        } else if (fl.l_type == F_WRLCK) {
            printf("=> Write lock on character %d; PID: %d\n", offset, fl.l_pid);
        }
    }

    return 0;
}

void readFromFile(int fd, short offset) {

    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf("Error while seeking the offset.\n");
        return;
    }

    char character;
    if (read(fd, &character, sizeof(char)) == -1) {
        printf("Error while reading from file.\n");
        return;
    }

    printf("Character with offset %d:  %c\n", offset, character);
}

int writeToFile(int fd, short offset, char character) {

    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf("Error while seeking the offset.\n");
        return -1;
    }

    if (write(fd, &character, sizeof(char)) == -1) {
        printf("Error while writing to file.\n");
        return -1;
    }
    printf("Character with offset %d set to:  %c\n", offset, character);
    return 0;
}

int main(int argc, char **argv) {
    char *fileName;

    if (argc < 2) {
        fileName = "genFile";
    } else {
        fileName = argv[1];
    }

    int fileDescriptor = open(fileName, O_RDWR);
    if (fileDescriptor == -1) {
        printf("Cannot open file %s.", fileName);
        return -2;
    }

    short offset;
    char *type = malloc(sizeof(char) * 64);
    char *cmd = (char *) malloc(sizeof(char) * 64);
    char character;
    int result;

    printf("Type help to see the list of possible actions.\n");

    while (strcmp(cmd, "exit") != 0) {
        scanf("%s", cmd);

        if (strcmp(cmd, "help") == 0) {
            printf("usage:\n");
            printf("\tlock <position> [r|w|u]\n");
            printf("\t\t- read lock(r), read and write lock(w), unlock(u) character at <position>\n");
            printf("\tdisplay - show available locks\n");
            printf("\tread <position> - read character at <position>\n");
            printf("\twrite position char - write <char> at <position>\n");
            printf("\texit - exit program\n");

        } else if (strcmp(cmd, "lock") == 0) {
            scanf("%hi", &offset);
            scanf("%s", type);

            if (strcmp(type, "r") == 0) {
                result = setLock(fileDescriptor, offset, F_RDLCK);
            } else if (strcmp(type, "w") == 0) {
                result = setLock(fileDescriptor, offset, F_WRLCK);
            } else if (strcmp(type, "u") == 0) {
                result = setLock(fileDescriptor, offset, F_UNLCK);
            } else {
                printf("Wrong option. Type help to see the list of possible actions.\n");
                result = -2;
            }

            if(result >= 0) {
                printf("Lock set on %d. character.\n", result);
            } else {
                printf("Error while locking.\n");
            };

        } else if (strcmp(cmd, "display") == 0) {

            showLocked(fileDescriptor);
            printf("END\n");

        } else if (strcmp(cmd, "read") == 0) {

            scanf("%hi", &offset);
            readFromFile(fileDescriptor, offset);

        } else if (strcmp(cmd, "write") == 0) {

            scanf(" %hi", &offset);
            scanf(" %c", &character);
            writeToFile(fileDescriptor, offset, character);

        } else if (strcmp(cmd, "exit") != 0) {
            printf("Wrong option. Type help to see the list of possible actions.\n");
        }
    }

    close(fileDescriptor);
    free(type);
    free(cmd);

    return 0;
}