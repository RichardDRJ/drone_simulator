/* user includes */
#include "util/port_numbers.h"
#include "util/error.h"
#include "util/server_init.h"
#include "util/config.h"
#include "control/control_server.h"
#include "control/vrep_control.h"
#include "control/control_handlers.h"
#include "control/control_messages.h"
#include "data_structures/trie.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

static struct trie control_command_trie;

void create_control_command_trie(void)
{
    bzero(&control_command_trie, sizeof(control_command_trie));

    insert_to_trie(&control_command_trie, "AT*REF", &control_ref_handler);
    insert_to_trie(&control_command_trie, "AT*PCMD", &control_pcmd_handler);
    insert_to_trie(&control_command_trie, "AT*PCMD_MAG", &control_pcmd_mag_handler);
    insert_to_trie(&control_command_trie, "AT*FTRIM", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*CONFIG", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*CONFIG_IDS", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*COMWDG", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*CALIB", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*CTRL", &control_ctrl_handler);

    /*struct trie *config_trie = get_config_trie();

    insert_to_trie(config_trie, "control:euler_angle_max", &control_euler_max_handler);
    insert_to_trie(config_trie, "control:control_vz_max", &control_vz_max_handler);*/
}

void *control_listen(void *args)
{
    struct server_init *server_init = (struct server_init*)args;
    int listen_port = server_init->port;

    struct control_session_data td = {.done = 0,
        .buf_size = 300,
        .bytes_left = 0,
        .seq_num = 0,
        .len = sizeof(td.serv_addr),
        .max_roll = 0.4f,
        .max_pitch = 0.4f,
        .max_vert_speed = 1000.0f,
        .max_ang_speed = 1.0f,
        .at_pcmd_mag = server_init->d->at_pcmd_mag,
        .at_ref = server_init->d->at_ref,
    };
    
    td.buffer = malloc(sizeof(char) * td.buf_size);

    td.sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    td.serv_addr.sin_family = AF_INET;
    td.serv_addr.sin_addr.s_addr = INADDR_ANY;
    td.serv_addr.sin_port = htons(listen_port);

    if (bind(td.sockfd, (struct sockaddr *)&td.serv_addr, td.len) < 0) 
        error("ERROR on binding");

    struct trie_node *n = control_command_trie.root;

    while(!td.done)
    {
        if(!td.bytes_left)
        {
            td.bytes_left = recvfrom(td.sockfd, td.buffer, td.buf_size, 0, NULL, NULL);

            if(td.bytes_left < 1)
                error("ERROR reading from socket");

            td.buf_ptr = td.buffer;
        }
        else
        {
            if(*td.buf_ptr == '=')
            {
                if(n && n->handler)
                {
                    ++td.buf_ptr;
                    --td.bytes_left;
                    n->handler(&td);
                    n = control_command_trie.root;
                    
                    continue;
                }

                n = control_command_trie.root;
            }
            else
            {
                n = traverse_to_child_char(*td.buf_ptr, n);

                if(!n)
                    n = control_command_trie.root;
            }

            ++td.buf_ptr;
            --td.bytes_left;
        }
    }
    
    free(td.buffer);

    return NULL;
}
