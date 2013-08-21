#include "util/config.h"
#include "data_structures/trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct trie opt_trie = {.root = NULL};

struct trie *get_config_trie(void)
{
    return &opt_trie;
}

void config_set_option(char *param_name, char *param_value, struct control_session_data *session_data)
{
    handler_t handler = insert_kv_pair_to_trie(&opt_trie, param_name, param_value);
    struct config_handler_data d = {
        .session_data = session_data,
        .value = param_value,
    };
    if(handler)
        handler(&d);
}

void config_read_options(void)
{
    FILE *f = fopen("configuration", "rb");
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);
    char *buffer = malloc(sz);
    fread(buffer, sz, 1, f);

    char *saveptr = NULL;
    char key[41];
    char value[81];

    memset(key, 0, 41);
    memset(value, 0, 81);

    char *token;
    char *buf_ptr = buffer;

    while( (token = strtok_r(buf_ptr, "\n", &saveptr)) )
    {
        sscanf(token, "%40s%*[\t ]=%*[\t ]%80c\n",key,value);
        insert_kv_pair_to_trie(&opt_trie, key, value);

        buf_ptr = NULL;
    }

    free(buffer);
    fclose(f);
}

void config_write_options(void)
{
    FILE *f = fopen("configuration", "wb");

    struct trie_node *saveptr = NULL;

    char *key;
    char *value;

    while(iterate_key_value_pairs(&opt_trie, &saveptr, &key, &value))
    {
        fwrite(key, strlen(key), 1, f);
        fwrite("\t=\t", 3, 1, f);
        fwrite(value, strlen(value), 1, f);
    }

    fclose(f);
}
