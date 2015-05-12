#ifndef _COMMONS_H_
#define _COMMONS_H_

#define SERVER_QUEUE_NAME "/server"
#define MAX_CLIENTS 30
#define HSQ_SIZE 10

typedef struct message {
	char time[25];
	char to[128];
	char content[256];
} message;

#endif