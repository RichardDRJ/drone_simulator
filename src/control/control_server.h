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

    uint8_t done;

    void (*atref)(uint8_t start, uint8_t select);

    uint32_t seq_num;
};

#endif
