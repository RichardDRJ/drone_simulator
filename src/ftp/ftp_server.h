#ifndef FTP_SERVER_H
#define FTP_SERVER_H

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

void create_ftp_command_trie(void);
void *ftp_listen(void*);
void *ftp_session(void*);
void *ftp_data_listen(void*);

struct session_data
{
    char type;
    char *current_username;

    char *filename;

    int client_sockfd;
    int server_sockfd;

    uint8_t done;

    int data_sockfd;
    int data_client_sockfd;
    struct sockaddr_in data_sock;
    pthread_t data_thread;
    pthread_mutex_t retr_mutex;
};

#endif
