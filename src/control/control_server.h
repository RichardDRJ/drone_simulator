#ifndef CONTROL_SERVER_H
#define CONTROL_SERVER_H

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

void create_control_command_trie(void);
void *control_listen(void*);
void *control_session(void*);
void *control_data_listen(void*);

struct control_session_data
{
    int sockfd;
    struct sockaddr_in serv_addr;
    socklen_t len;

    uint8_t done;

    void (*at_ref)(struct control_session_data *d, uint8_t start, uint8_t select);
    void (*at_pcmd_mag)(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed, float magneto_psi, float magneto_psi_accuracy);

    uint32_t seq_num;

    int16_t buf_size;
    int16_t bytes_left;
    char *buffer;
    char *buf_ptr;

    float max_roll;
    float max_pitch;
    float max_vert_speed;
    float max_ang_speed;
};

#endif
