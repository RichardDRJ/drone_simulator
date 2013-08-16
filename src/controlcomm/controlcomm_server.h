#ifndef CONTROLCOMM_SERVER_H
#define CONTROLCOMM_SERVER_H

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

void *controlcomm_listen(void *args);

struct controlcomm_session_data
{
    int server_sockfd;
    struct sockaddr_in serv_addr;
    socklen_t servlen;

    int client_sockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen;

    uint8_t done;
};

#endif
