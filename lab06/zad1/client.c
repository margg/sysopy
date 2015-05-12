#include <sys/types.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "commons.h"

int pid;
int status;
int senderId = 0;
int receiverId = 0;
message msg;
time_t currentTime;
struct tm *timeStruct;
char timeString[25];
idStruct clientId;

void signalHandler(int signal);

void receiveMessages();

void sendMessages();

void exitClient();

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please provide the username.\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, signalHandler);
    signal(SIGTSTP, signalHandler);
    signal(SIGTERM, signalHandler);

    key_t key = ftok(SERVER_LOCATION, 1);
    if (key == -1) {
        perror("Getting key failed.");
        exit(EXIT_FAILURE);
    }

    int mainQueue = msgget(key, 0);

    if (mainQueue == -1) {
        perror("Connection to server failed.");
        exit(EXIT_FAILURE);
    }

    char *username = argv[1];
    clientId.mtype = CLIENT;
    sprintf(clientId.id, "%s", username);

    status = msgsnd(mainQueue, &clientId, sizeof(clientId.id), 0);
    sleep(1);

    if (status == -1) {
        perror("Logging to server failed. Try again later.");
        exit(EXIT_FAILURE);
    }

    //receiver queue id
    status = (int) msgrcv(mainQueue, &clientId, sizeof(clientId.id), SERVER, 0);

    if (status == -1) {
        perror("Communication with server failed.");
        exit(EXIT_FAILURE);
    }

    receiverId = atoi(clientId.id);

    //sender queue id
    status = (int) msgrcv(mainQueue, &clientId, sizeof(clientId.id), SERVER, 0);

    if (status == -1) {
        perror("Communication with server failed.");
        exit(EXIT_FAILURE);
    }
    senderId = atoi(clientId.id);

    printf("Logged in successfully.\n");

    pid = fork();

    if (pid == -1) {
        perror("Function fork() failed.");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        receiveMessages();
    } else {
        sendMessages();
    }

    return 0;
}

void receiveMessages() {
    while (1) {

        status = (int) msgrcv(receiverId, &msg, sizeof(msg), SERVER, 0);
        if (status != -1) {
            printf("\n\t***\n");
            printf("\t%s  %s \t%s", msg.time, msg.to, msg.content);
            printf("\t***\n");
        }

        sleep(1);
    }
}

void sendMessages() {

    while (1) {
        printf("To: ");
        fgets(msg.to, sizeof(msg.to), stdin);

        while (strcmp(msg.to, "\n") == 0) {
            fgets(msg.to, sizeof(msg.to), stdin);
        }

        printf("Message: ");
        fgets(msg.content, sizeof(msg.content), stdin);

        if (strcmp(msg.content, "exit\n") == 0) {
            exitClient();
        }

        time(&currentTime);
        timeStruct = localtime(&currentTime);
        strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", timeStruct);

        strcpy(msg.time, timeString);
        msg.mtype = CLIENT;

        status = msgsnd(senderId, &msg, sizeof(msg), 0);

        if (status == -1) {
            perror("Sending message failed.");
        }
    }
}

void exitClient() {
    printf("Client exitting.\n\n");
    kill(pid, SIGKILL);
    exit(EXIT_SUCCESS);
}

void signalHandler(int signal) {
    kill(pid, SIGKILL);
    exit(EXIT_SUCCESS);
}
