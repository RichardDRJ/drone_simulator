/* User includes */
#include "ftp_handlers.h"
#include "ftp_server.h"
#include "ftp_messages.h"
#include "util/error.h"

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <strings.h>
#include <string.h>

/* Networking includes */

#include <sys/socket.h>
#include <netinet/in.h>

char *read_args(int sockfd)
{
    uint32_t buffsize = 256;
    char *args_buffer = calloc(sizeof(char), buffsize);
    uint32_t index = 0;

    while(1)
    {
        uint8_t cr = 0;

        while(index < buffsize - 1)
        {
            int8_t bytes_read = recv(sockfd, &args_buffer[index], 1, 0);

            if(bytes_read < 1)
                error("ERROR reading from socket");
            else if(cr && args_buffer[index] == '\n')
            {
                args_buffer[index - 1] = 0;
                break;
            }
            else if(args_buffer[index] == '\r')
            {
                cr = 1;
                ++index;
                continue;
            }

            cr = 0;

            ++index;
        }

        if(index == buffsize)
        {
            buffsize *= 2;
            char *tmp_args_buffer = calloc(sizeof(char), buffsize);
            memcpy(tmp_args_buffer, args_buffer, buffsize / 2);
            free(args_buffer);
            args_buffer = tmp_args_buffer;
            continue;
        }

        break;
    }

    return args_buffer;
}

void ftp_size_handler(void *data)
{
    struct session_data *d = data;
    char *args = read_args(d->client_sockfd);
    char *tokeniser_saveptr;

    char *filename = strtok_r(args, "\r\n ", &tokeniser_saveptr);

    FILE *file = fopen(filename, "rb");

    if(!file)
    {
        size_t message_size = strlen(MSG_NO_SUCH_FILE);
        write(d->client_sockfd, MSG_NO_SUCH_FILE, message_size);
    }
    else
    {
        fseek(file, 0L, SEEK_END);
        size_t sz = ftell(file);
        fclose(file);

        size_t message_size = sizeof(MSG_TELL_SIZE) + 10;
        char ret_message[message_size];
        bzero(ret_message, message_size);

        message_size = snprintf(ret_message, message_size, MSG_TELL_SIZE " %zu\r\n", sz);

        write(d->client_sockfd, ret_message, message_size);
    }

    free(args);
}

void ftp_type_handler(void *data)
{
    struct session_data *d = data;
    char *args = read_args(d->client_sockfd);
    char *tokeniser_saveptr;

    char *newtype = strtok_r(args, "\r\n ", &tokeniser_saveptr);
    d->type = *newtype;

    size_t message_size = strlen(MSG_OPERATION_SUCCESS);

    write(d->client_sockfd, MSG_OPERATION_SUCCESS, message_size);

    free(args);
}

void ftp_user_handler(void *data)
{
    struct session_data *d = data;
    char *args = read_args(d->client_sockfd);
    char *tokeniser_saveptr;

    char *uname = strtok_r(args, "\r\n ", &tokeniser_saveptr);

    if(!uname || !strcmp(uname, "anonymous"))
    {
        d->current_username = "anonymous";
        write(d->client_sockfd, MSG_LOGIN_SUCCESS, strlen(MSG_LOGIN_SUCCESS));
    }

    free(args);
}

void ftp_pasv_handler(void *data)
{
    struct session_data *d = data;
    char ret_message[sizeof(MSG_PASSIVE_SUCCESS) + 29];

    d->data_sock.sin_port = 0;

    if (bind(d->data_sockfd, (struct sockaddr *)&d->data_sock, sizeof(d->data_sock)) < 0) 
        error("ERROR on PASV binding");

    listen(d->data_sockfd, 5);

    socklen_t len = sizeof(d->data_sock);
    if (getsockname(d->data_sockfd, (struct sockaddr *)&d->data_sock, &len) == -1)
        error("getsockname");

    pthread_create(&d->data_thread, NULL, ftp_data_listen, d);

    snprintf(ret_message, sizeof(MSG_PASSIVE_SUCCESS) + 29, MSG_PASSIVE_SUCCESS " (%d,%d,%d,%d,%d,%d)\r\n", 192, 168, 1, 1, 255 & d->data_sock.sin_port, 255 & (d->data_sock.sin_port >> 8));

    printf("%s\n", ret_message);

    write(d->client_sockfd, ret_message, strlen(ret_message));
}

void ftp_quit_handler(void *data)
{
    struct session_data *d = data;
    d->done = 1;
    write(d->client_sockfd, MSG_QUIT_SUCCESS, strlen(MSG_QUIT_SUCCESS));
    close(d->client_sockfd);
}

void ftp_empty_handler(void *data)
{
    struct session_data *d = data;
    write(d->client_sockfd, MSG_UNSUPPORTED, strlen(MSG_UNSUPPORTED));
}

void ftp_retr_handler(void *data)
{
    struct session_data *d = data;
    char *args = read_args(d->client_sockfd);
    char *tokeniser_saveptr;

    d->filename = strtok_r(args, "\r\n ", &tokeniser_saveptr);

    pthread_mutex_unlock(&d->retr_mutex);

    pthread_join(d->data_thread, NULL);

    struct sockaddr_in client_sock;
    socklen_t len = sizeof(client_sock);
    if (getsockname(d->client_sockfd, (struct sockaddr *)&client_sock, &len) == -1)
        error("getsockname");

    printf("client_sock port: %d\n", ntohs(client_sock.sin_port));

    int bytes_written = write(d->client_sockfd, MSG_RETR_SUCCESS, strlen(MSG_RETR_SUCCESS));
    if (bytes_written < 0)
        error("ERROR writing to socket");

    free(args);
}
