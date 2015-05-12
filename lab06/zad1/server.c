#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include "commons.h"

int mainQueue;
int *sendQueues;
int *receiveQueues;
char **clientId;
int connectedClients = 0;
int status = 0;
int senderId = 0;
int receiverId = 0;
char username[128];
message msg;

void sigHandler(int signal);

int getQueue(char *);

void closeQueue(char *, int);

void connectNewClient();

void processMessages();

int main(int argc, char *argv[]) {

    signal(SIGINT, sigHandler);
    signal(SIGTSTP, sigHandler);
    signal(SIGTERM, sigHandler);

    sendQueues = malloc(MAX_CLIENTS * sizeof(mqd_t));
    receiveQueues = malloc(MAX_CLIENTS * sizeof(mqd_t));
    clientId = malloc(MAX_CLIENTS * sizeof(char *));

    mainQueue = msgget(ftok(SERVER_LOCATION, 1), IPC_CREAT | 0644);

    if (mainQueue == -1) {
        printf("Creating server failed.");
        exit(EXIT_FAILURE);
    }

    printf("Server running.\n");

    while (1) {
        sleep(1);

        connectNewClient();
        processMessages();
    }

    return 0;
}

void connectNewClient() {

    idStruct idData;

    status = (int) msgrcv(mainQueue, &idData, sizeof(idData.id), 0, IPC_NOWAIT);

    if (status != -1) {

        sprintf(username, "%s", idData.id);

        senderId = msgget(IPC_PRIVATE, IPC_CREAT | 0644);

        if (senderId == -1) {
            perror("Server error while creating sending message queue.");
            exit(EXIT_FAILURE);
        }

        receiverId = msgget(IPC_PRIVATE, IPC_CREAT | 0644);

        if (receiverId == -1) {
            perror("Server error while creating receiving message queue.");
            exit(EXIT_FAILURE);
        }

        idData.mtype = SERVER;

        //sending id of client's receiving channel
        sprintf(idData.id, "%d", senderId);
        msgsnd(mainQueue, &idData, sizeof(idData.id), 0);

        //sending id of client's sender channel
        sprintf(idData.id, "%d", receiverId);
        msgsnd(mainQueue, &idData, sizeof(idData.id), 0);

        sendQueues[connectedClients] = senderId;
        receiveQueues[connectedClients] = receiverId;
        clientId[connectedClients] = malloc(sizeof(char) * strlen(username));
        sprintf(clientId[connectedClients], "%s\n", username);

        printf("New user registered: %s\n", username);
        connectedClients++;

    }
}

void processMessages() {
    int i;
    for (i = 0; i < connectedClients; i++) {
        status = (int) msgrcv(receiveQueues[i], &msg, sizeof(msg), 0, IPC_NOWAIT);

        if (status != -1) {

            int receiverQueue = getQueue(msg.to);

            if (receiverQueue == -1) {
                sprintf(msg.content, "No such client. Please try again.\n");
                receiverQueue = sendQueues[i]; // message is sent back to the sender
            } else {
                sprintf(msg.to, "%s", clientId[i]);
            }

            msg.mtype = SERVER;
            status = msgsnd(receiverQueue, &msg, sizeof(msg), 0);

            if (status < 0) {
                perror("Sending a message failed. Please try again.\n");
            }

        }
    }
}

int getQueue(char *receiverName) {
    int i;
    for (i = 0; i < connectedClients; i++) {
        if (strcmp(receiverName, clientId[i]) == 0) {
            return sendQueues[i];
        }
    }

    return -1;
}

void sigHandler(int signal) {

    closeQueue(SERVER_LOCATION, mainQueue);

    int i;
    for (i = 0; i < connectedClients; i++) {
        closeQueue(clientId[i], sendQueues[i]);
        free(clientId[i]);
    }

    free(sendQueues);
    free(receiveQueues);
    free(clientId);

    exit(EXIT_SUCCESS);
}

void closeQueue(char *name, int queue) {
    msgctl(queue, IPC_RMID, NULL);
    unlink(name);
}
