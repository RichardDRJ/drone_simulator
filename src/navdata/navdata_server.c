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

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(listen_port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, 5);

    int8_t bytes_read;
    
    uint32_t sequence = 0;
    
    struct sockaddr_in client_addr;
    socklen_t client_length = sizeof(struct sockaddr_in);
    
    int navdata_size = sizeof(navdata_t) + sizeof(navdata_demo_t) + sizeof(navdata_cks_t) - sizeof(navdata_option_t);
    navdata_t *navdata = calloc(navdata_size, 1);
    
    do {
        bytes_read = recvfrom(sockfd, &sequence, sizeof(sequence), 0, (struct sockaddr*)&client_addr, &client_length);

        if(bytes_read < 1)
            error("ERROR reading from socket");
    } while(!bytes_read);

    while(1)
    {
        navdata->header = NAVDATA_HEADER;
        navdata->ardrone_state = ARDRONE_NAVDATA_DEMO_MASK;
        navdata->sequence = sequence++;
        navdata->vision_defined = 0;
        
        navdata_demo_t *demo = (navdata_demo_t*)(&navdata->options[0]);
        server_init->d->fill_navdata_demo(demo);

/*        demo->tag = NAVDATA_DEMO_TAG;
        demo->ctrl_state = 0;
        demo->vbat_flying_percentage = 0xFFFFFFFF;
        demo->theta = 0;
        demo->phi = 0;
        demo->psi = 0;
        demo->altitude = 0;

        demo->vx = 0;
        demo->vy = 0;
        demo->vz = 0;

        demo->num_frames = 0;
        
        demo->size = sizeof(navdata_demo_t);*/
        
        navdata_cks_t *cks = (navdata_cks_t*)(demo + 1);
        cks->tag = NAVDATA_CKS_TAG;
        cks->size = sizeof(navdata_cks_t);
        
        uint32_t i = 0;
        uint32_t checksum = 0;
        
        for(i = 0; i < navdata_size - sizeof(navdata_cks_t); ++i)
            checksum += ((uint8_t*)navdata)[i];
        
        cks->cks = checksum;

        sendto(sockfd, navdata, navdata_size, 0, (struct sockaddr*)&client_addr, client_length);
    }
    
    free(navdata);

    return NULL;
}
