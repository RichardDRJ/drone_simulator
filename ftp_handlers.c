/* User includes */
#include "ftp_handlers.h"
#include "ftp_messages.h"
#include "error.h"

/* Standard includes */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

static char *current_user = NULL;

void user_handler(int sockfd)
{
    char uname_buffer[256];
    bzero(uname_buffer, 256);
    uint8_t done = 0;
    int16_t index = -1;
    uint8_t cr = 0;

    while(++index < 255)
    {
        int8_t bytes_read = recv(sockfd, &uname_buffer[index], 1, 0);

        if(bytes_read < 1)
        {
            error("ERROR reading from socket");
            return;
        }

        if(uname_buffer[index] == ' ')
        {
            index = -1;
            continue;
        }

        if(cr && uname_buffer[index] == '\n')
        {
            uname_buffer[index - 1] = 0;
            break;
        }

        if(uname_buffer[index] == '\r')
        {
            cr = 1;
            continue;
        }
        else
            cr = 0;
    }

    if(!strcmp(uname_buffer, "anonymous") || uname_buffer[0] == 0)
    {
        current_user = "anonymous";
        write(sockfd, MSG_LOGIN_SUCCESS, sizeof(MSG_LOGIN_SUCCESS));
    }
}

void port_handler(int);
void pasv_handler(int);

void empty_handler(int sockfd)
{
    write(sockfd, MSG_UNSUPPORTED, strlen(MSG_UNSUPPORTED));
}
