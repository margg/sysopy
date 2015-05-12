#ifndef _COMMONS_H_
#define _COMMONS_H_

#define SERVER_LOCATION "."
#define MAX_CLIENTS 30

#define CLIENT 1
#define SERVER 2

typedef struct message {
    long mtype;
    char time[25];
    char to[128];
    char content[256];
} message;

typedef struct idStruct {
    long mtype;
    char id[128];
} idStruct;

#endif