/* User includes */
#include "socket_numbers.h"

/* Standard includes */
#include <string.h>
#include <stdio.h>
//#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

#define MSG_OPERATION_OK "220 Operation successful\r\n"

uint8_t done = 0;

void ftp_listen(void)
{
    char buffer[256];
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(FTP_LISTEN_SOCK);

    if (bind(server_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        printf("ERROR on binding\n");
    listen(server_sockfd, 5);

    int client_sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(client_sockfd < 0)
        printf("ERROR on accept\n");

    while(!done)
    {
        bzero(buffer,256);

        /*  Right now this will receive null if there isn't a new packet waiting but if we make it wait, it waits for 255 characters. */
        if (recv(client_sockfd, buffer, 255, MSG_DONTWAIT) < 0)
        {
            if(!(errno & (EAGAIN | EWOULDBLOCK)))
            {
                printf("ERROR reading from socket\n");
                return;
            }
        }

        int bytes_written = write(client_sockfd,MSG_OPERATION_OK,strlen(MSG_OPERATION_OK));
        if (bytes_written < 0)
            printf("ERROR writing to socket\n");
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
        printf("ERROR on binding\n");
    listen(server_sockfd, 5);

    clilen = sizeof(cli_addr);
    int client_sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(client_sockfd < 0)
        printf("ERROR on accept\n");

    bzero(buffer,256);

    int bytes_written = read(client_sockfd,buffer,255);
    if (bytes_written < 0)
        printf("ERROR reading from socket\n");
    printf("Here is the message: %s\n",buffer);

    bytes_written = write(client_sockfd,"I got your message",18);
    if (bytes_written < 0)
        printf("ERROR writing to socket\n");

    close(client_sockfd);
    close(server_sockfd);
    return 0; 
}
