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


struct sockaddr_in serverSocketInet;
struct sockaddr_un clientSocketUnix;
int socketFd;
char username[USERNAME_MAX_LEN];
int keyboardDescriptors[2];



int getInetSocket(char *IPaddr, int port);

int getUnixSocket(char *localServerPath);

void clientRun(int mode);

void exitClient();

void getMessage(char *msg);

void *handleInput(void *arg);

void sendRequest(Message *message);

void destroyClient(int arg);

void sendMessage(Message *message);

void receiveMessage(Message *message);

void printUsage(char *progName);



int main(int argc, char *argv[]) {

    if (argc < 4) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(username, argv[1]);
    char *serverType = argv[2];
    char *IPaddr = DEFAULT_ADDRESS;
    int port = DEFAULT_PORT;
    char *localServerPath = LOCAL_SERVER_PATH;
    int clientMode = MODE_UNIX;

    if (strcmp(serverType, "U") == 0) {
        localServerPath = malloc(strlen(argv[3]) * sizeof(char));
        strcpy(localServerPath, argv[3]);
        clientMode = MODE_UNIX;
    } else if (strcmp(serverType, "N") == 0) {
        IPaddr = argv[3];
        port = atoi(argv[4]);
        clientMode = MODE_INET;
    } else {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, destroyClient);
    signal(SIGTERM, destroyClient);

    if (clientMode == MODE_INET) {
        socketFd = getInetSocket(IPaddr, port);
    } else {
        socketFd = getUnixSocket(localServerPath);
    }

    clientRun(clientMode);

    close(socketFd);
    return 0;
}

void printUsage(char *progName) {
    printf("\nInvalid arguments.\n\nUsage:\n");
    printf("-> for local UNIX connection:\n \t%s\t<username> U <local server socket path>\n", progName);
    printf("-> for remote INET connection:\n \t%s\t<username> N <IP> <port>\n\n", progName);
}

void clientRun(int mode) {

    Message message;
    sprintf(message.username,"%s", username);

    pthread_t thr;
    pthread_create(&thr, NULL, handleInput, NULL);
    fd_set descriptorsSet;

    printf("Logged in as %s.\n", username);
    printf("You can start typing your messages.\nType \"logout\" to exit.\n\n");

    while (1) {
        FD_ZERO(&descriptorsSet);
        FD_SET(keyboardDescriptors[0], &descriptorsSet);
        FD_SET(socketFd, &descriptorsSet);

        select(USERS_MAX + 3, &descriptorsSet, NULL, NULL, NULL);

        if (FD_ISSET(keyboardDescriptors[0], &descriptorsSet)) {
            sendMessage(&message);
        } else if (FD_ISSET(socketFd, &descriptorsSet)) {
            receiveMessage(&message);
            printf("%s: %s", message.username, message.msg);
        }
    }

}

void receiveMessage(Message *message) {
    if (recv(socketFd, message, sizeof(Message), 0) < 0) {
        perror("Error : Could not receive message from server. ");
        exitClient();
    }
}

void sendMessage(Message *message) {
    char msg[MSG_MAX_LEN];
    getMessage(msg);
    sprintf(message->username, username);

    if(strcmp(msg, "logout\n") == 0) {
        destroyClient(1);
    } else if (strcmp(msg, "\n") != 0) {
        sprintf(message->msg, "%s", msg);
        sendRequest(message);
    }
}

void sendRequest(Message *message) {
    if (write(socketFd, message, sizeof(Message)) < 0) {
        perror("Error : Could not send message to server. ");
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
    sprintf(path, "%s", localServerPath);

    if ((socketFd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Error : Could not create socket. ");
        exit(EXIT_FAILURE);
    }



    clientSocketUnix.sun_family = AF_UNIX;
    strncpy(clientSocketUnix.sun_path, path, strlen(path));

    if (connect(socketFd, (struct sockaddr *) &clientSocketUnix, sizeof(clientSocketUnix)) < 0) {
        perror("Error : Connection failure. ");
        exitClient();
    }

    printf("Connected to Unix socket.\n");
    return socketFd;
}

int getInetSocket(char *IPaddr, int port) {
    int socketFd;

    if ((socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Error : Could not create socket. ");
        exit(EXIT_FAILURE);
    }

    serverSocketInet.sin_family = AF_INET;
    serverSocketInet.sin_port = htons(port);
    if (inet_pton(AF_INET, IPaddr, &serverSocketInet.sin_addr) < 0) {
        perror("Error : Connection failure. ");
        exitClient();
    }

    if (connect(socketFd, (struct sockaddr *) &clientSocketUnix, sizeof(clientSocketUnix)) < 0) {
        perror("Error : Connection failure. ");
        exitClient();
    }

    printf("Connected to Internet socket.\n");
    return socketFd;
}

void destroyClient(int arg) {
    Message message;
    sprintf(message.username, "%s", username);
    sprintf(message.msg, "logout");

    sendRequest(&message);

    printf("Logout successfull. See you!\n\n");
    shutdown(socketFd, SHUT_RDWR);
    exitClient();
}