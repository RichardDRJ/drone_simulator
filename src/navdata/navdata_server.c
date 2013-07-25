/* user includes */
#include "util/port_numbers.h"
#include "util/error.h"
#include "navdata/navdata_server.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void *navdata_listen(void *args)
{
    int listen_port = *(int*)args;

    struct sockaddr_in serv_addr;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(listen_port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, 5);

    char current_char;

    while(1)
    {
        int8_t bytes_read = recv(sockfd, &current_char, 1, 0);

        if(bytes_read < 1)
        {
            error("ERROR reading from socket");
            return NULL;
        }
    }

    return NULL;
}
