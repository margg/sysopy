#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/select.h>
#include <signal.h>

#include "commons.h"


struct sockaddr *serverSocketAddr;
struct sockaddr_in serverSocketInet;
struct sockaddr_un serverSocket;
struct sockaddr_un clientSocketUnix;
int socketFd;
int id;
char username[USERNAME_MAX_LEN];


int keyboardDescriptors[2];

void loginUser(int mode, char *username);

int getInetSocket(char *IPaddr, int port);

int getUnixSocket(char *localServerPath);

void clientRun(int mode);

void exitClient();

void getMessage(char *msg);

void *handleInput(void *arg);

void sendKeepAliveMessage(Request *req);

void sendRequest(Request *req);

void destroyClient(int arg);


void sendMessage(Request *req);

void receiveMessage(Message *message);

int main(int argc, char *argv[]) {


    strcpy(username, "marg333");
    int serverType = AF_INET;
    char *IPaddr = DEFAULT_ADDRESS;
    int port = DEFAULT_PORT;
    char *localServerPath = LOCAL_SERVER_PATH;

    signal(SIGINT, destroyClient);
    signal(SIGTERM, destroyClient);

    int clientMode = MODE_UNIX;

    if (clientMode == MODE_INET) {
        socketFd = getInetSocket(IPaddr, port);
    } else {
        socketFd = getUnixSocket(localServerPath);
    }

    loginUser(clientMode, username);
    clientRun(clientMode);

    close(socketFd);
    return 0;
}

void clientRun(int mode) {

    Request req;
    sprintf(req.username, "%s", username);
    req.id = id;
    req.mode = mode;
    req.size = sizeof(clientSocketUnix);
    Message message;

    pthread_t thr;
    pthread_create(&thr, NULL, handleInput, NULL);
    fd_set descriptorsSet;

    printf("You can start typing your messages.\nType \"logout\" to exit.\n\n");

    while (1) {
        FD_ZERO(&descriptorsSet);
        FD_SET(keyboardDescriptors[0], &descriptorsSet);
        FD_SET(socketFd, &descriptorsSet);

        struct timeval timeout;
        timeout.tv_sec = CLIENT_TIMEOUT;
        timeout.tv_usec = 0;

        int readyDescriptors = select(USERS_MAX + 3, &descriptorsSet, NULL, NULL, &timeout);

        if (!readyDescriptors) {
            sendKeepAliveMessage(&req);
        } else if (FD_ISSET(keyboardDescriptors[0], &descriptorsSet)) {
            sendMessage(&req);
        }

        if (FD_ISSET(socketFd, &descriptorsSet)) {
            receiveMessage(&message);
            printf("%s: %s", message.username, message.msg);
        }
    }

}

void receiveMessage(Message *message) {
    if (recvfrom(socketFd, message, sizeof(Message), 0, NULL, NULL) < 0) {
        perror("Error : Could not receive message from server. ");
        exitClient();
    }
}

void sendMessage(Request *req) {
    char msg[MSG_MAX_LEN];
    getMessage(msg);

    if (strcmp(msg, "logout\n") == 0) {
        (*req).type = REQ_LOGOUT;
        sprintf((*req).msg, "");

        sendRequest(req);

        printf("Logout successfull. See you!\n\n");
        shutdown(socketFd, SHUT_RDWR);
        exitClient();
    }

    req->type = REQ_MESSAGE;
    sprintf(req->msg, "%s", msg);
    sendRequest(req);
}

void sendKeepAliveMessage(Request *req) {
    req->type = REQ_REGISTER;
    sendRequest(req);
}

void sendRequest(Request *req) {
    if (sendto(socketFd, req, sizeof(Request), 0, serverSocketAddr, (socklen_t) req->size) < 0) {
        perror("Error : Could not send request to server. ");
        exitClient();
    }
}

void *handleInput(void *arg) {

    pipe(keyboardDescriptors);
    char buffer[MSG_MAX_LEN];
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        if (write(keyboardDescriptors[1], buffer, MSG_MAX_LEN) < 0) {
            perror("Error : Could not write to file.");
        }
    }
}

void getMessage(char *msg) {
    read(keyboardDescriptors[0], msg, MSG_MAX_LEN);
    msg[MSG_MAX_LEN - 1] = '\0';
}

void exitClient() {
    close(socketFd);
    exit(EXIT_SUCCESS);
}

int getUnixSocket(char *localServerPath) {
    char path[PATH_MAX_LEN];
    sprintf(path, "/tmp/c%i", getpid());

    unlink(path);

    if ((socketFd = socket(PF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("Could not create socket!\n");
        exit(EXIT_FAILURE);
    }

    clientSocketUnix.sun_family = AF_UNIX;
    strncpy(clientSocketUnix.sun_path, path, strlen(path));

    if (bind(socketFd, (struct sockaddr *) &clientSocketUnix, sizeof(clientSocketUnix)) < 0) {
        printf("Connection failure.\n");
        exit(EXIT_FAILURE);
    }

    serverSocket.sun_family = AF_UNIX;
    strcpy(serverSocket.sun_path, localServerPath);
    serverSocketAddr = (struct sockaddr *) &serverSocket;

    printf("Unix socket created.\n");
    return socketFd;
}

int getInetSocket(char *IPaddr, int port) {
    int socketFd;

    if ((socketFd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Could not create socket!\n");
        exit(EXIT_FAILURE);
    }

    serverSocketInet.sin_family = AF_INET;
    serverSocketInet.sin_port = htons(port);
    if (inet_pton(AF_INET, IPaddr, &serverSocketInet.sin_addr) < 0) {
        printf("Connection failure.\n");
        exitClient();
    }

    serverSocketAddr = (struct sockaddr *) &serverSocketInet;

    printf("Internet socket created.\n");
    return socketFd;
}

void loginUser(int mode, char *username) {

    Request req;
    sprintf(req.username, "%s", username);
    req.id = -1;
    req.type = REQ_REGISTER;
    req.mode = mode;
    req.size = sizeof(clientSocketUnix);

    if (sendto(socketFd, &req, sizeof(Request), 0, serverSocketAddr, sizeof(serverSocket)) < 0) {
        perror("Error : Could not login to server. ");
        close(socketFd);
        exit(EXIT_FAILURE);
    }

    if (recvfrom(socketFd, &id, sizeof(int), 0, serverSocketAddr, (socklen_t *) &req.size) < 0) {
        perror("Error : Could not receive id from server. \n");
        exit(EXIT_FAILURE);
    }

    printf("Login successfull. Logged in with id %d\n", id);
}

void destroyClient(int arg) {
    printf("Client destroyed.\n");
    shutdown(socketFd, SHUT_RDWR);
    exitClient();
}