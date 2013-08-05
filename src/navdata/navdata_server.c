/* user includes */
#include "util/port_numbers.h"
#include "util/error.h"
#include "util/server_init.h"
#include "navdata/navdata_server.h"
#include "navdata/navdata_common.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void *navdata_listen(void *args)
{
    struct server_init *server_init = (struct server_init*)args;
    int listen_port = server_init->port;

    struct sockaddr_in serv_addr;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(listen_port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, 5);

    char current_char;
    int8_t bytes_read = recv(sockfd, &current_char, 1, 0);

    if(bytes_read < 1)
    {
        error("ERROR reading from socket");
        return NULL;
    }

//    uint32_t sequence = 0;

    while(1)
    {
        /*int navdata_size = sizeof(navdata_t) + sizeof(navdata_demo_t) + sizeof(navdata_cks_t) - sizeof(navdata_option_t);
        navdata_t *navdata = malloc(navdata_size);

        navdata->header = NAVDATA_HEADER;
        navdata->ardrone_state = 0;
        navdata->sequence = sequence++;

        navdata->ctrl_state = 0;
        navdata->vbat_flying_percentage = 0xFFFFFFFF;
        navdata->theta = 0;
        navdata->phi = 0;
        navdata->psi = 0;
        navdata->altitude = 0;

        navdata->vx = 0;
        navdata->vy = 0;
        navdata->vz = 0;

        navdata->num_frames = 0;

        write(sockfd, navdata, navdata_size);*/
    }

    return NULL;
}
