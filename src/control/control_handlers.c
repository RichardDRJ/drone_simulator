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
#include <errno.h>

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
    char *buf_start = session_data->buf_ptr;

    while(1)
    {
        if(!session_data->bytes_left)
        {
            errno = ENOBUFS;
            error("ERROR buffer too small");
        }
        else
        {
            printf("%c", *session_data->buf_ptr);
            if(*session_data->buf_ptr == '\r')
                break;

            --session_data->bytes_left;
            ++session_data->buf_ptr;
        }
    }

    printf("\n");

    return buf_start;
}

void control_ref_handler(void *arg)
{
    struct control_session_data *session_data = arg;
    char *args = control_read_args(session_data);

    uint32_t seq_num;
    uint32_t control;

    sscanf(args, "%" SCNu32 ",%" SCNu32 "\r", &seq_num, &control);
    
    if(seq_num >= session_data->seq_num)
    {
        session_data->at_ref(session_data, (control >> 9) & 1, (control >> 8) & 1);
        session_data->seq_num = seq_num;
    }
}

void control_pcmd_handler(void *arg)
{
    struct control_session_data *session_data = arg;
    char *args = control_read_args(session_data);

    uint32_t seq_num;
    uint32_t control;
    float roll;
    float pitch;
    float vert_speed;
    float ang_speed;

    sscanf(args, "%" SCNu32 ",%" SCNu32 ",%d,%d,%d,%d\r", &seq_num, &control, (int*)&roll, (int*)&pitch, (int*)&vert_speed, (int*)&ang_speed);

    if(seq_num >= session_data->seq_num)
    {
        session_data->at_pcmd_mag(session_data, control, roll, pitch, vert_speed, ang_speed, 0.0f, 0.0f);
        session_data->seq_num = seq_num;
    }
}

void control_pcmd_mag_handler(void *arg)
{
    struct control_session_data *session_data = arg;
    char *args = control_read_args(session_data);

    uint32_t seq_num;
    uint32_t control;
    float roll;
    float pitch;
    float vert_speed;
    float ang_speed;
    float magneto_psi;
    float magneto_psi_accuracy;

    sscanf(args, "%" SCNu32 ",%" SCNu32 ",%d,%d,%d,%d,%d,%d\r", &seq_num, &control, (int*)&roll, (int*)&pitch, (int*)&vert_speed, (int*)&ang_speed, (int*)&magneto_psi, (int*)&magneto_psi_accuracy);

    if(seq_num >= session_data->seq_num)
    {
        session_data->at_pcmd_mag(session_data, control, roll, pitch, vert_speed, ang_speed, magneto_psi, magneto_psi_accuracy);
        session_data->seq_num = seq_num;
    }
}
