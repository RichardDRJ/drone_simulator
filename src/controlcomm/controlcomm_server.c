/* user includes */
#include "util/port_numbers.h"
#include "util/error.h"
#include "util/server_init.h"
#include "controlcomm/controlcomm_server.h"
#include "data_structures/linked_list.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

struct linked_list data_list;
sem_t data_semaphore;

static void *controlcomm_session(void *args);

void *controlcomm_listen(void *args)
{
    struct server_init *server_init = (struct server_init*)args;
    int listen_port =server_init->port;

    struct controlcomm_session_data td = {.done = 0,
        .servlen = sizeof(td.serv_addr),
        .clilen = sizeof(td.cli_addr),
    };
    
    td.server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    td.serv_addr.sin_family = AF_INET;
    td.serv_addr.sin_addr.s_addr = INADDR_ANY;
    td.serv_addr.sin_port = htons(listen_port);

    sem_init(&data_semaphore, 0, 0);

    if (bind(td.server_sockfd, (struct sockaddr *)&td.serv_addr, td.servlen) < 0) 
        error("ERROR on binding");

    listen(td.server_sockfd, 5);

    while(!td.done)
    {
        td.client_sockfd = accept(td.server_sockfd, (struct sockaddr *) &td.cli_addr, &td.clilen);
        if(td.client_sockfd < 0)
            error("ERROR on accept");

        struct controlcomm_session_data *session_data = malloc(sizeof(struct controlcomm_session_data));
        *session_data = td;

        pthread_t session_thread;
        pthread_create(&session_thread, NULL, controlcomm_session, session_data);
    }

    return NULL;
}

static void *controlcomm_session(void *args)
{
    struct controlcomm_session_data *session_data = args;

    while(1)
    {
        sem_wait(&data_semaphore);
        struct list_elem *e = list_pop(&data_list);
        void *data = e->data;
        size_t length = e->length;
        int bytes_written = write(session_data->client_sockfd, data, length);

        if(bytes_written < 0)
            error("ERROR writing to socket");

        free(data);
        free(e);
    }

    return NULL;
}
