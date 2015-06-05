#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/un.h>
#include <stdio.h>
#include <time.h>
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

void serveRequest(int requestedSocket);

int isMessageAuthor(int i, int requestUserId);

int getAvailableIndex();

void removeInactiveClients();

void logoutUser(int id);

int registerNewClient(int requestedSocket, Request *req, struct sockaddr *clientSocket, size_t sockLength);

void updateUserActivity(int id);



int main(int argc, char **argv) {

    if (argc < 3) {
        printf("\nInvalid arguments.\n\nUsage:\n");
        printf("\t%s \t<port> <local server socket path>", argv[0]);
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
    printf("Server initialized.\n");

    fd_set descriptorsSet;

    while (1) {

        FD_ZERO(&descriptorsSet);
        FD_SET(unixSocket, &descriptorsSet);
        FD_SET(inetSocket, &descriptorsSet);

        struct timeval timeout;
        timeout.tv_sec = SERVER_TIMEOUT;
        timeout.tv_usec = 0;

        select(USERS_MAX + 3, &descriptorsSet, NULL, NULL, &timeout);

        removeInactiveClients();

        if (FD_ISSET(unixSocket, &descriptorsSet)) {
            serveRequest(unixSocket);
        }
        if (FD_ISSET(inetSocket, &descriptorsSet)) {
            serveRequest(inetSocket);
        }
    }

    return 0;
}

int createUnixSocket(char *socketFilePath) {

    struct sockaddr_un socketAddr;
    int socketFd;

    unlink(socketFilePath);

    if ((socketFd = socket(PF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("Could not create socket!\n");
        exit(EXIT_FAILURE);
    }

    socketAddr.sun_family = AF_UNIX;
    strcpy(socketAddr.sun_path, socketFilePath);

    if (bind(socketFd, (struct sockaddr *) &socketAddr, (socklen_t) sizeof(socketAddr)) < 0) {
        perror("Connection failure.\n");
        exit(EXIT_FAILURE);
    }

    printf("Unix socket created.\n");
    return socketFd;
}

int createInetSocket(int port) {

    struct sockaddr_in socketAddr;
    int socketFd;

    if ((socketFd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Could not create socket!\n");
        exit(EXIT_FAILURE);
    }

    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(port);
    socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketFd, (struct sockaddr *) &socketAddr, sizeof(socketAddr)) < 0) {
        printf("Connection failure.\n");
        exit(EXIT_FAILURE);
    }

    printf("Internet socket created.\n");
    return socketFd;
}

void serveRequest(int requestedSocket) {

    Request req;
    struct sockaddr clientSocket;
    socklen_t sockLength = (socklen_t) sizeof(clientSocket);
    Message msg;

    if (recvfrom(requestedSocket, &req, sizeof(Request), 0, &clientSocket, &sockLength) < 0) {
        perror("Error : No pending requests. ");
        return;
    }

    switch (req.type) {
        case REQ_REGISTER:
            if (req.id == -1) {
                printf("Received register new client request.\n");
                if(registerNewClient(requestedSocket, &req, &clientSocket, sockLength) == -1) {
                    printf("Server busy. Skipping registration of new client.");
                    break;
                }
            } else {
                printf("Received activity request from %s.\n", req.username);
                updateUserActivity(req.id);
            }
            break;
        case REQ_LOGOUT:
            printf("Received logout request from user %s.\n", req.username);
            if (users[req.id] != NULL) {
                logoutUser(req.id);
            } else {
                printf("Error : User not connected \n");
            }
            break;
        case REQ_MESSAGE:
            printf("Received message request from user %s (%d): %s", req.username, req.id, req.msg);

            snprintf(msg.username, USERNAME_MAX_LEN - 1, "%s", req.username);
            sprintf(msg.msg, "%s", req.msg);
            msg.username[USERNAME_MAX_LEN - 1] = '\0';
            msg.msg[MSG_MAX_LEN - 1] = '\0';

            for (int i = 0; i < USERS_MAX; i++) {
                if (users[i] != NULL && (users[i]->id != req.id)) {
                    clientSocket = *(users[i]->clientSocket);
                    sockLength = (socklen_t) users[i]->size;
                    if (sendto(requestedSocket, &msg, sizeof(Message), 0, &clientSocket, sockLength) < 0) {
                        printf("Could not send message to %s (ID: %d).\n", msg.username, users[i]->id);
                    } else {
                        printf("Message sent to: %s (%d)\n", users[i]->username, users[i]->id);
                    }
                }
            }
            updateUserActivity(req.id);
            break;
        default:
            printf("Unknown request received.\n");
            break;
    }

}

void removeInactiveClients() {
    for (int i = 0; i < USERS_MAX; i++) {
        if (users[i] != NULL && (time(NULL) - users[i]->lastActivityTime) > SERVER_TIMEOUT) {
            printf("Logging out user: %s (%d)\n", users[i]->username, users[i]->id);
            logoutUser(i);
        }
    }
}

void updateUserActivity(int id) {
    if(users[id] != NULL) {
        users[id]->lastActivityTime = time(NULL);
    } else {
        perror("Updating null user!\n");
    }
}

int registerNewClient(int requestedSocket, Request *req, struct sockaddr *clientSocket, size_t sockLength) {
    if ((userIndex = getAvailableIndex()) == -1) {
        perror("Error : Server busy. Please try again later. ");
        return -1;
    }

    users[userIndex] = (User *) malloc(sizeof(User));
    users[userIndex]->id = userIndex;
    sprintf(users[userIndex]->username, "%s", req->username);
    users[userIndex]->mode = req->mode;
    users[userIndex]->size = req->size;
    users[userIndex]->clientSocket = (struct sockaddr *) malloc(sizeof(struct sockaddr));
    memcpy(users[userIndex]->clientSocket, clientSocket, sockLength);

    updateUserActivity(users[userIndex]->id);

    if (sendto(requestedSocket, &userIndex, sizeof(userIndex), 0, clientSocket, (socklen_t) req->size) == -1) {
        perror("Error : Could not send ID to new user.\n");
    }

    return userIndex;
}

void logoutUser(int id) {
    if (users[id] != NULL) {
        free(users[id]->clientSocket);
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
    close(unixSocket);
    close(inetSocket);
    for (i = 0; i < USERS_MAX; ++i) {
        if (users[i] != NULL) {
            free(users[i]->clientSocket);
            free(users[i]);
        }
    }
    unlink(socketFilePath);
    free(socketFilePath);

    printf("Server destroyed.\n");

    exit(EXIT_SUCCESS);
}




