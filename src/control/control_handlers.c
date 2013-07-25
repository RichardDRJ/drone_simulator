/* User includes */
#include "control_handlers.h"
#include "control_server.h"
#include "control_messages.h"
#include "util/error.h"

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <strings.h>
#include <string.h>

/* Networking includes */

#include <sys/socket.h>
#include <netinet/in.h>

void control_empty_handler(void *arg)
{
    printf("Empty handler\n");
}

static char *control_read_args(struct control_session_data *session_data)
{
    //  Doesn't work because now the packet is read into a buffer.
    uint16_t buf_size = 200;
    char *buffer = malloc(sizeof(char) * buf_size);
    char *curr_char = buffer;

    while(1)
    {
        int8_t bytes_read = recv(session_data->sockfd, curr_char, 1, 0);

        if(bytes_read < 1)
            error("ERROR reading from socket");
        else if(bytes_read == 1 && *curr_char++ == '\r')
            break;
    }

    return buffer;
}

void control_ref_handler(void *arg)
{
    struct control_session_data *session_data = arg;
    char *args = control_read_args(session_data);

    uint32_t seq_num;
    uint32_t control;

    sscanf(args, "%" SCNu32 ",%" SCNu32 "\r", &seq_num, &control);
    
    printf("REF handler\n");

    if(seq_num > session_data->seq_num)
    {
        session_data->atref((control >> 9) & 1, (control >> 8) & 1);
        session_data->seq_num = seq_num;
    }
}

void control_pcmd_handler(void *arg)
{
    struct control_session_data *session_data = arg;
    char *args = control_read_args(session_data);

    uint32_t seq_num;
    uint32_t control;
    float left_right;
    float front_back;
    float vert_speed;
    float ang_speed;

    sscanf(args, "%" SCNu32 ",%" SCNu32 ",%d,%d,%d,%d\r", &seq_num, &control, (int*)&left_right, (int*)&front_back, (int*)&vert_speed, (int*)&ang_speed);

    if(seq_num > session_data->seq_num)
    {
//        session_data->atref((control >> 9) & 1, (control >> 8) & 1);
        printf("%d,%d,%f,%f,%f,%f\n", seq_num, control, left_right, front_back, vert_speed, ang_speed);
        session_data->seq_num = seq_num;
    }
}
