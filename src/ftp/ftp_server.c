/* user includes */
#include "util/port_numbers.h"
#include "util/error.h"
#include "util/server_init.h"
#include "ftp/ftp_server.h"
#include "ftp/ftp_handlers.h"
#include "ftp/ftp_messages.h"
#include "data_structures/trie.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

static struct trie ftp_command_trie;

void create_ftp_command_trie(void)
{
    bzero(&ftp_command_trie, sizeof(ftp_command_trie));

    /* Access control commands */
    insert_to_trie(&ftp_command_trie, "USER", &ftp_user_handler);
    insert_to_trie(&ftp_command_trie, "PASS", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "ACCT", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "CWD", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "CDUP", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "SMNT", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "REIN", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "QUIT", &ftp_quit_handler);

    /* Transfer parameter commands */
    insert_to_trie(&ftp_command_trie, "PORT", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "PASV", &ftp_pasv_handler);
    insert_to_trie(&ftp_command_trie, "TYPE", &ftp_type_handler);
    insert_to_trie(&ftp_command_trie, "STRU", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "MODE", &ftp_empty_handler);

    /* FTP service commands */
    insert_to_trie(&ftp_command_trie, "RETR", &ftp_retr_handler);
    insert_to_trie(&ftp_command_trie, "STOR", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "ATOU", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "APPE", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "ALLO", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "REST", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "RNFR", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "RNTO", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "ABOR", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "DELE", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "RMD", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "MKD", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "PWD", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "LIST", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "NLST", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "SITE", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "SYST", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "STAT", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "HELP", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "NOOP", &ftp_empty_handler);

    /* FTP extensions */
    insert_to_trie(&ftp_command_trie, "SIZE", &ftp_size_handler);
    insert_to_trie(&ftp_command_trie, "MTDM", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "MLST", &ftp_empty_handler);
    insert_to_trie(&ftp_command_trie, "MLSD", &ftp_empty_handler);
}

void *ftp_listen(void *args)
{
    struct server_init *server_init = (struct server_init*)args;
    int listen_port = server_init->port;

    struct session_data td = {.type = 'A',
        .retr_mutex = PTHREAD_MUTEX_INITIALIZER,
        .data_sock = {  .sin_family = AF_INET,
            .sin_addr = {.s_addr = INADDR_ANY},
            .sin_port = htons(20)},
        .data_sockfd = socket(AF_INET, SOCK_STREAM, 0),
        .done = 0,
    };

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(listen_port);

    if (bind(server_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(server_sockfd, 5);

    while(1)
    {
        int client_sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if(client_sockfd < 0)
            error("ERROR on accept");

        int bytes_written = write(client_sockfd,MSG_OPERATION_SUCCESS,strlen(MSG_OPERATION_SUCCESS));
        if (bytes_written < 0)
            error("ERROR writing to socket");

        td.client_sockfd = client_sockfd;
        td.server_sockfd = server_sockfd;

        struct session_data *session_data = malloc(sizeof(struct session_data));
        *session_data = td;

        pthread_t session_thread;
        pthread_create(&session_thread, NULL, ftp_session, session_data);
    }
}

void *ftp_session(void *args)
{
    struct session_data *session_data = (struct session_data*)args;

    struct trie_node *n = ftp_command_trie.root;
    uint8_t cr = 0;
    char current_char;

    pthread_mutex_lock(&session_data->retr_mutex);

    while(!session_data->done)
    {
        int8_t bytes_read = recv(session_data->client_sockfd, &current_char, 1, 0);

        if(bytes_read < 1)
        {
            error("ERROR reading from socket");
            return NULL;
        }

        if(current_char == '\r')
        {
            cr = 1;
            continue;
        }
        else if(cr && current_char == '\n')
        {
            n = ftp_command_trie.root;
            continue;
        }

        n = traverse_to_child_char(current_char, n);

        if(!n)
            n = ftp_command_trie.root;
        else if(n->handler)
        {
            printf("Command: %s\n", n->key);
            n->handler(session_data);
            n = ftp_command_trie.root;
        }
    }

    free(session_data);

    return NULL;
}

void *ftp_data_listen(void *args)
{
    struct session_data *d = (struct session_data*)args;

    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    int data_client_sockfd = accept(d->data_sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(data_client_sockfd < 0)
        error("ERROR on data accept");

    pthread_mutex_lock(&d->retr_mutex);

    FILE *f = fopen(d->filename, "rb");

    if(!f)
    {
        size_t message_size = strlen(MSG_NO_SUCH_FILE);
        write(d->client_sockfd, MSG_NO_SUCH_FILE, message_size);
    }
    else
    {
        fseek (f , 0 , SEEK_END);
        size_t size = ftell (f);
        rewind (f);

        char *buffer = malloc(size);

        fread(buffer, 1, size, f);

        fclose(f);

        int bytes_written = write(d->client_sockfd,MSG_OPENING_BINARY_CONN, strlen(MSG_OPENING_BINARY_CONN));
        if (bytes_written < 0)
            error("ERROR writing to socket");

        bytes_written = send(data_client_sockfd, buffer, size, 0);
        if (bytes_written < 0)
            error("ERROR writing to socket");

        free(buffer);
    }

    close(data_client_sockfd);

    pthread_mutex_unlock(&d->retr_mutex);

    return NULL;
}
