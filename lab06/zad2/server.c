#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <mqueue.h>
#include <stdio.h>
#include "commons.h"

mqd_t mainQueue;
mqd_t *sendQueues;
mqd_t *receiveQueues;
char **clientId;
int connectedClients = 0;
int status = 0;
mqd_t senderId = 0;
mqd_t receiverId = 0;

char username[128];
char buff[128];
message msg;
struct mq_attr attr;

void sigHandler(int signal);
int getQueue(char *);
void closeQueue(char *, mqd_t);
void connectNewClient();
void processMessages();

int main(int argc, char* argv[]) {

    signal(SIGINT, sigHandler);
    signal(SIGTSTP, sigHandler);
    signal(SIGTERM, sigHandler);

    sendQueues = malloc(MAX_CLIENTS * sizeof(mqd_t));
    receiveQueues = malloc(MAX_CLIENTS * sizeof(mqd_t));
    clientId = malloc(MAX_CLIENTS * sizeof(char *));

    attr.mq_maxmsg = HSQ_SIZE;
    attr.mq_msgsize = sizeof(char) * 128;
    attr.mq_flags = 0;

    mainQueue = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT | O_NONBLOCK, 0664, &attr);
    if(mainQueue == -1) {
        perror("Creating server failed.");
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

    status = (int) mq_receive(mainQueue, (char*) username, sizeof(username), NULL);

    if(status != -1) {
        attr.mq_maxmsg = HSQ_SIZE;
        attr.mq_msgsize = sizeof(msg);
        attr.mq_flags = 0;

        // username_s as name for msg queue for sending data
        sprintf(buff, "/%s_s", username);
        senderId = mq_open(buff, O_RDWR | O_CREAT | O_NONBLOCK, 0644, &attr);
        if(senderId == -1) {
            perror("Server error while creating sending message queue.");
            exit(EXIT_FAILURE);
        }

        // username_r as name for msg queue for receiving data
        sprintf(buff, "/%s_r", username);
        receiverId = mq_open(buff, O_RDWR | O_CREAT | O_NONBLOCK, 0644, &attr);
        if(receiverId == -1) {
            perror("Server error while creating receiving message queue.");
            exit(EXIT_FAILURE);
        }

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
    for(i = 0; i < connectedClients; i++) {

        status = (int) mq_receive(receiveQueues[i], (char*)&msg, sizeof(msg), NULL);

        if(status != -1) {
            int receiverQueue = getQueue(msg.to);

            if(receiverQueue == -1) {
                sprintf(msg.content, "No such client.\n");
                receiverQueue = sendQueues[i]; // message is sent back to the sender
            } else {
                sprintf(msg.to, "%s", clientId[i]);
            }

            status = mq_send(receiverQueue, (char*)&msg, sizeof(msg), 0);

            if(status == -1) {
                perror("Sending message failed.\n");
            }
        }
    }
}

int getQueue(char *receiverName) {
    int i;
    for(i = 0; i < connectedClients; i++) {
        if(strcmp(receiverName, clientId[i]) == 0) {
            return sendQueues[i];
        }
    }

    return -1;
}

void sigHandler(int signal) {

    closeQueue(SERVER_QUEUE_NAME, mainQueue);

    int i;
    for(i = 0; i < connectedClients; i++) {
        closeQueue(clientId[i], sendQueues[i]);
        free(clientId[i]);
    }

    free(sendQueues);
    free(receiveQueues);
    free(clientId);

    printf("\nServer terminating.\n\n");

    exit(EXIT_SUCCESS);
}

void closeQueue(char *name, mqd_t queue) {
    mq_close(queue);
    mq_unlink(name);
}
