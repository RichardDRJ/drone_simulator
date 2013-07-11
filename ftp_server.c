/* user includes */
#include "socket_numbers.h"
#include "error.h"
#include "ftp_server.h"
#include "ftp_handlers.h"
#include "ftp_messages.h"
#include "trie.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

static struct trie ftp_command_trie;
extern struct sockaddr_in data_sock;
extern int data_sockfd;

void create_ftp_command_trie(void)
{
    bzero(&ftp_command_trie, sizeof(ftp_command_trie));

    /* Access control commands */
    insert_to_trie(&ftp_command_trie, "USER", &user_handler);
    insert_to_trie(&ftp_command_trie, "PASS", &empty_handler);
    insert_to_trie(&ftp_command_trie, "ACCT", &empty_handler);
    insert_to_trie(&ftp_command_trie, "CWD", &empty_handler);
    insert_to_trie(&ftp_command_trie, "CDUP", &empty_handler);
    insert_to_trie(&ftp_command_trie, "SMNT", &empty_handler);
    insert_to_trie(&ftp_command_trie, "REIN", &empty_handler);
    insert_to_trie(&ftp_command_trie, "QUIT", &empty_handler);

    /* Transfer parameter commands */
    insert_to_trie(&ftp_command_trie, "PORT", &empty_handler);
    insert_to_trie(&ftp_command_trie, "PASV", &pasv_handler);
    insert_to_trie(&ftp_command_trie, "TYPE", &empty_handler);
    insert_to_trie(&ftp_command_trie, "STRU", &empty_handler);
    insert_to_trie(&ftp_command_trie, "MODE", &empty_handler);

    /* FTP service commands */
    insert_to_trie(&ftp_command_trie, "RETR", &empty_handler);
    insert_to_trie(&ftp_command_trie, "STOR", &empty_handler);
    insert_to_trie(&ftp_command_trie, "ATOU", &empty_handler);
    insert_to_trie(&ftp_command_trie, "APPE", &empty_handler);
    insert_to_trie(&ftp_command_trie, "ALLO", &empty_handler);
    insert_to_trie(&ftp_command_trie, "REST", &empty_handler);
    insert_to_trie(&ftp_command_trie, "RNFR", &empty_handler);
    insert_to_trie(&ftp_command_trie, "RNTO", &empty_handler);
    insert_to_trie(&ftp_command_trie, "ABOR", &empty_handler);
    insert_to_trie(&ftp_command_trie, "DELE", &empty_handler);
    insert_to_trie(&ftp_command_trie, "RMD", &empty_handler);
    insert_to_trie(&ftp_command_trie, "MKD", &empty_handler);
    insert_to_trie(&ftp_command_trie, "PWD", &empty_handler);
    insert_to_trie(&ftp_command_trie, "LIST", &empty_handler);
    insert_to_trie(&ftp_command_trie, "NLST", &empty_handler);
    insert_to_trie(&ftp_command_trie, "SITE", &empty_handler);
    insert_to_trie(&ftp_command_trie, "SYST", &empty_handler);
    insert_to_trie(&ftp_command_trie, "STAT", &empty_handler);
    insert_to_trie(&ftp_command_trie, "HELP", &empty_handler);
    insert_to_trie(&ftp_command_trie, "NOOP", &empty_handler);

    /* FTP extensions */
    insert_to_trie(&ftp_command_trie, "SIZE", &size_handler);
    insert_to_trie(&ftp_command_trie, "MTDM", &empty_handler);
    insert_to_trie(&ftp_command_trie, "MLST", &empty_handler);
    insert_to_trie(&ftp_command_trie, "MLSD", &empty_handler);
}

void ftp_listen(void)
{
    data_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    data_sock.sin_port = htons(20);

//    char buffer[6];
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(FTP_LISTEN_SOCK);

    if (bind(server_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(server_sockfd, 5);

    int client_sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(client_sockfd < 0)
        error("ERROR on accept");

    int bytes_written = write(client_sockfd,MSG_OPERATION_SUCCESS,strlen(MSG_OPERATION_SUCCESS));
    if (bytes_written < 0)
        error("ERROR writing to socket");

    char current_char;

//    bzero(buffer,6);
//    char *curr_buffer_loc = &buffer[0];

    uint8_t done = 0;
    struct trie_node *n = ftp_command_trie.root;
    uint8_t cr = 0;

    while(!done)
    {
        int8_t bytes_read = recv(client_sockfd, &current_char, 1, 0);

        if(bytes_read < 1)
        {
            error("ERROR reading from socket");
            return;
        }

        //printf("char: %c\n", current_char);

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
            n->handler(client_sockfd);
            n = ftp_command_trie.root;
        }

/*        printf("Reading\n");
        int bytes_read = recv(client_sockfd, buffer, 255, 0);
        printf("Read %s\n", buffer);
        if(bytes_read < 0)
                printf("ERROR reading from socket\n");
*/

        /*  TODO: Search trie for FTP commands with a handler function for each. If there is no handler function, do nothing. If the sent code is invalid, close connection. */
    }

    close(client_sockfd);
    close(server_sockfd);
}
