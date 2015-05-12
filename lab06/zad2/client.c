#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <stdio.h>
#include "commons.h"

int pid;
int status;
mqd_t receiverId = 0, senderId = 0;
message msg;
time_t currentTime;
struct tm *timeStruct;
char timeString[25];
struct mq_attr attr;
mqd_t mainQueue;
char *username;

void sigHandler(int signal);

void receiveMessages();

void sendMessages();

void connectToServer();

void createQueues();

void exitClient();

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please provide the username.\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigHandler);
    signal(SIGTSTP, sigHandler);
    signal(SIGTERM, sigHandler);

    username = argv[1];
    connectToServer();

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

void connectToServer() {

    attr.mq_maxmsg = HSQ_SIZE;
    attr.mq_msgsize = sizeof(char) * 128;
    attr.mq_flags = 0;

    mainQueue = mq_open(SERVER_QUEUE_NAME, O_WRONLY, 0664, &attr);

    if (mainQueue == -1) {
        perror("Connection to server failed.");
        exit(EXIT_FAILURE);
    }

    status = mq_send(mainQueue, username, sizeof(char) * 128, 0);

    if (status == -1) {
        perror("Logging to server failed. Try again later.");
        exit(EXIT_FAILURE);
    }

    createQueues();
}

void createQueues() {
    attr.mq_maxmsg = HSQ_SIZE;
    attr.mq_msgsize = sizeof(msg);
    attr.mq_flags = 0;
    char buffer[128];

    //receiver queue id
    sprintf(buffer, "/%s_s", username);
    receiverId = mq_open(buffer, O_RDWR | O_CREAT | O_NONBLOCK, 0664, &attr);
    if (receiverId == -1) {
        printf("Communication with server failed.");
        exit(EXIT_FAILURE);
    }

    //sender queue id
    sprintf(buffer, "/%s_r", username);
    senderId = mq_open(buffer, O_RDWR | O_CREAT | O_NONBLOCK, 0664, &attr);
    if (senderId == -1) {
        printf("Communication with server failed.");
        exit(EXIT_FAILURE);
    }

    printf("Logged in successfully.\n");
}

void receiveMessages() {
    while (1) {

        status = (int) mq_receive(receiverId, (char *) (&msg), sizeof(msg), NULL);
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

        status = mq_send(senderId, (char *) (&msg), sizeof(msg), 0);

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

void sigHandler(int signal) {
    kill(pid, SIGKILL);
    exit(EXIT_SUCCESS);
}
