/* User includes */
#include "control_handlers.h"
#include "control_server.h"
#include "control_messages.h"
#include "util/error.h"
#include "data_structures/linked_list.h"
#include "util/config.h"

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

extern struct linked_list data_list;
extern sem_t data_semaphore;

void control_empty_handler(void *arg)
{
    printf("Empty handler\n");
}

static char *control_read_args(struct control_session_data *session_data)
{
    char *buf_start = session_data->buf_ptr;

    while(1)
    {
        if(!session_data->bytes_left)
        {
            errno = ENOBUFS;
            error("ERROR");
        }
        else
        {
            if(*session_data->buf_ptr == '\r')
                break;

            --session_data->bytes_left;
            ++session_data->buf_ptr;
        }
    }

    return buf_start;
}

void control_ctrl_handler(void *arg)
{
    struct control_session_data *session_data = arg;
    char *args = control_read_args(session_data);

    uint32_t seq_num;
    uint32_t control;
    FILE *f;

    sscanf(args, "%" SCNu32 ",%" SCNu32 "\r", &seq_num, &control);
    
    switch(control)
    {
        case 4:
            f = fopen("configuration", "rb");
            fseek(f, 0L, SEEK_END);
            size_t sz = ftell(f);
            fseek(f, 0L, SEEK_SET);
            char *buffer = malloc(sz);
            fread(buffer, sz, 1, f);
            list_add(&data_list, buffer, sz);
            sem_post(&data_semaphore);
            fclose(f);
            break;
        default:
            break;
    }
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

void control_euler_max_handler(void *aux)
{
    struct config_handler_data *d = aux;
    float value = atof(d->value);
    d->session_data->max_roll = value;
    d->session_data->max_pitch = value;
}

void control_vz_max_handler(void *aux)
{
    struct config_handler_data *d = aux;
    float value = atof(d->value);
    d->session_data->max_vert_speed = value;
}

void control_yaw_handler(void *aux)
{
    struct config_handler_data *d = aux;
    float value = atof(d->value);
    d->session_data->max_ang_speed = value;
}

void control_config_handler(void *arg)
{
    struct control_session_data *session_data = arg;
    char *args = control_read_args(session_data);

    uint32_t seq_num;
    char param_name[41];
    char param_value[81];

    sscanf(args, "%" SCNu32 ",%40s,%80s\r", &seq_num, param_name, param_value);
    
    if(seq_num >= session_data->seq_num)
    {
        config_set_option(param_name, param_value, session_data);
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
