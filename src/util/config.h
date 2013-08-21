#ifndef UTIL_CONFIG_H
#define UTIL_CONFIG_H

#include "../control/control_server.h"

struct config_handler_data
{
    struct control_session_data *session_data;
    char *value;
};

void config_set_option(char *param_name, char *param_value, struct control_session_data *session_data);
void config_read_options(void);
void config_write_options(void);
struct trie *get_config_trie(void);

#endif
