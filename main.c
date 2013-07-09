/* User includes */
#include "socket_numbers.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

#define MSG_OPERATION_OK "220 Operation successful\r\n"

uint8_t done = 0;

void error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void ftp_listen(void)
{
    char buffer[6];
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

    int bytes_written = write(client_sockfd,MSG_OPERATION_OK,strlen(MSG_OPERATION_OK));
    if (bytes_written < 0)
        error("ERROR writing to socket");


    char current_char;

    bzero(buffer,6);
//    char *curr_buffer_loc = &buffer[0];

    while(!done)
    {
        printf("Reading\n");
        int8_t bytes_read = recv(client_sockfd, buffer, 1, 0);
        printf("Read: %c\n", buffer[0]);

        if(bytes_read < 1)
        {
            error("ERROR reading from socket");
            return;
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

int main(int argc, char *argv)
{
    ftp_listen();

    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(AUTH_SOCK);

    if (bind(server_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding\n");
    listen(server_sockfd, 5);

    clilen = sizeof(cli_addr);
    int client_sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(client_sockfd < 0)
        error("ERROR on accept\n");

    bzero(buffer,256);

    int bytes_written = read(client_sockfd,buffer,255);
    if (bytes_written < 0)
        error("ERROR reading from socket\n");
    printf("Here is the message: %s\n",buffer);

    bytes_written = write(client_sockfd,"I got your message",18);
    if (bytes_written < 0)
        error("ERROR writing to socket\n");

    close(client_sockfd);
    close(server_sockfd);
    return 0; 
}
