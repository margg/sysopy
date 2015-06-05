#pragma once


#define MODE_UNIX 0
#define MODE_INET 1

#define REQ_REGISTER 0
#define REQ_LOGOUT 1
#define REQ_MESSAGE 2


#define USERS_MAX 100

#define LOCAL_SERVER_PATH "localServer"
#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 5432

#define CLIENT_TIMEOUT 10
#define SERVER_TIMEOUT 30

#define PATH_MAX_LEN 256
#define HOST_NAME_MAX_LEN 120
#define USERNAME_MAX_LEN 30
#define MSG_MAX_LEN 256


typedef struct {
    int id;
    char username[USERNAME_MAX_LEN];
    int mode;
    time_t lastActivityTime;
    size_t size;
    struct sockaddr *clientSocket;
} User;

typedef struct {
    int id;
    int mode;
    int type;
    int value;
    size_t size;
    char username[USERNAME_MAX_LEN];
    char msg[MSG_MAX_LEN];
} Request;

typedef struct {
    char username[USERNAME_MAX_LEN];
    char msg[MSG_MAX_LEN];
} Message;
