#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/select.h>

#include "commons.h"

int unixSocket = 0;
int inetSocket = 0;
int userIndex = 0;
User *users[USERS_MAX];
char socketFilePath[PATH_MAX_LEN];


void destroyServer(int arg);

int createUnixSocket(char *socketFilePath);

int createInetSocket(int port);

int getAvailableIndex();

void logoutUser(int id);

int registerNewClient(int clientFd);


int main(int argc, char **argv) {

    if (argc < 3) {
        printf("\nInvalid arguments.\n\nUsage:\n");
        printf("\t%s \t<port> <local server socket path>\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    sprintf(socketFilePath, "%s", argv[2]);

    signal(SIGINT, destroyServer);
    signal(SIGTERM, destroyServer);

    int i;
    for (i = 0; i < USERS_MAX; i++) {
        users[i] = NULL;
    }

    unlink(socketFilePath);

    unixSocket = createUnixSocket(socketFilePath);
    inetSocket = createInetSocket(port);

    listen(unixSocket, USERS_MAX);
    listen(inetSocket, USERS_MAX);
    printf("Server initialized.\n");

    fd_set descriptorsSet;

    while (1) {

        FD_ZERO(&descriptorsSet);
        FD_SET(unixSocket, &descriptorsSet);
        FD_SET(inetSocket, &descriptorsSet);

        for (i = 0; i < USERS_MAX; i++) {
            if (users[i] != NULL) {
                FD_SET(users[i]->descriptor, &descriptorsSet);
            }
        }

        select(USERS_MAX + 3, &descriptorsSet, NULL, NULL, NULL);

        if (FD_ISSET(unixSocket, &descriptorsSet)) {

            int clientFd = accept(unixSocket, NULL, NULL);
            registerNewClient(clientFd);
        }
        if (FD_ISSET(inetSocket, &descriptorsSet)) {

            int clientFd = accept(inetSocket, NULL, NULL);
            registerNewClient(clientFd);
        }

        Message message;

        for (i = 0; i < USERS_MAX; i++) {
            if (users[i] != NULL && FD_ISSET(users[i]->descriptor, &descriptorsSet)) {
                int len = (int) read(users[i]->descriptor, &message, sizeof(Message));
                if (len == 0) {
                    logoutUser(i);
                } else {
                    message.msg[MSG_MAX_LEN] = '\0';
                    printf("Received message:  %s : %s\n", message.username, message.msg);
                    int j;
                    for (j = 0; j < USERS_MAX; j++) {
                        if (users[j] != NULL && i != j) {
                            if (write(users[j]->descriptor, &message, sizeof(Message)) < 0) {
                                sprintf(stderr, "Could not send message to ID %d (msg:  %s: %s)\n", users[j]->id, message.username, message.msg);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int createUnixSocket(char *socketFilePath) {

    struct sockaddr_un socketAddr;
    int socketFd;

    unlink(socketFilePath);

    if ((socketFd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Error : Could not create socket. ");
        exit(EXIT_FAILURE);
    }


    int reuse = 1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Error : Could not set socket options. ");
        exit(EXIT_FAILURE);
    }

    socketAddr.sun_family = AF_UNIX;
    strcpy(socketAddr.sun_path, socketFilePath);

    if (bind(socketFd, (struct sockaddr *) &socketAddr, (socklen_t) sizeof(socketAddr)) < 0) {
        perror("Error : Could not bind the socket. ");
        exit(EXIT_FAILURE);
    }

    printf("Unix socket created.\n");
    return socketFd;
}

int createInetSocket(int port) {

    struct sockaddr_in socketAddr;
    int socketFd;

    if ((socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Error : Could not create socket. ");
        exit(EXIT_FAILURE);
    }

    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(port);
    socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketFd, (struct sockaddr *) &socketAddr, sizeof(socketAddr)) < 0) {
        perror("Error : Could not bind the socket. ");
        exit(EXIT_FAILURE);
    }

    printf("Internet socket created.\n");
    return socketFd;
}

int registerNewClient(int clientFd) {
    if ((userIndex = getAvailableIndex()) == -1) {
        fprintf(stderr, "Error : Server busy. Please try again later. ");
        return -1;
    }

    users[userIndex] = (User *) malloc(sizeof(User));
    users[userIndex]->id = userIndex;
    users[userIndex]->descriptor = clientFd;

    printf("Registered new user of ID: %d, desc: %d\n", userIndex, clientFd);

    return userIndex;
}

void logoutUser(int id) {
    if (users[id] != NULL) {
        printf("Logged out user of ID: %d, desc: %d\n", id, users[id]->descriptor);
        close(users[id]->descriptor);
        free(users[id]);
        users[id] = NULL;
    }
}

int getAvailableIndex() {
    for (int i = 0; i < USERS_MAX; i++) {
        if (users[i] == NULL) {
            return i;
        }
    }
    return -1;
}

void destroyServer(int arg) {
    int i;
    for (i = 0; i < USERS_MAX; ++i) {
        logoutUser(i);
    }
    close(unixSocket);
    close(inetSocket);
    unlink(socketFilePath);
    printf("Server destroyed.\n");
    exit(EXIT_SUCCESS);
}




