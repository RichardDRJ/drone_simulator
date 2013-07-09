/* User includes */
#include "socket_numbers.h"

/* Standard includes */
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv)
{
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
