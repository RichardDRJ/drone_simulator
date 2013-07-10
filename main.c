/* User includes */
#include "socket_numbers.h"
#include "error.h"
#include "ftp_server.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

uint8_t done = 0;

int main(int argc, char *argv)
{
    create_ftp_command_trie();
    ftp_listen();
}
