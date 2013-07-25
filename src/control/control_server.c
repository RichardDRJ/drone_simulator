/* user includes */
#include "util/port_numbers.h"
#include "util/error.h"
#include "control/control_server.h"
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
    insert_to_trie(&control_command_trie, "AT*PCMD_MAG", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*FTRIM", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*CONFIG", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*CONFIG_IDS", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*COMWDG", &control_empty_handler);
    insert_to_trie(&control_command_trie, "AT*CALIB", &control_empty_handler);
}

void *control_listen(void *args)
{
    int listen_port = *(int*)args;

    struct control_session_data td = {.done = 0,
        .seq_num = 0,
    };

    struct sockaddr_in serv_addr;

    td.sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(listen_port);

    if (bind(td.sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    struct trie_node *n = control_command_trie.root;

    socklen_t len = sizeof(serv_addr);
    uint16_t buf_size = 200;
    int16_t bytes_left = 0;
    char buffer[buf_size];
    char *buf_ptr = buffer;

    while(!td.done)
    {
        if(!bytes_left)
        {
            bytes_left = recvfrom(td.sockfd, buffer, buf_size, 0, (struct sockaddr *)&serv_addr, &len);

            printf("bytes_left: %d\n", bytes_left);

            if(bytes_left < 1)
                error("ERROR reading from socket");

            buf_ptr = buffer;
        }
        else
        {
            printf("read %c\n", *buf_ptr);

            if(*buf_ptr == '=')
            {
                if(n && n->handler)
                {
                    printf("Command: %s\n", n->key);
                    n->handler(&td);
                    n = control_command_trie.root;
                }

                n = control_command_trie.root;
            }
            else
            {
                n = traverse_to_child_char(*buf_ptr, n);

                if(!n)
                    n = control_command_trie.root;
            }

            ++buf_ptr;
            --bytes_left;
        }
    }

    return NULL;
}
