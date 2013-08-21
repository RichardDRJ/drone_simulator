#ifndef TRIE_H
#define TRIE_H

#include "ftp/ftp_server.h"
#include <stdint.h>

struct trie
{
    struct trie_node *root;
};

struct trie_node
{
    char c;

    struct trie_node *parent;
    uint8_t next_traverse;

    struct trie_node *left;
    struct trie_node *centre;
    struct trie_node *right;
    char *key;
    char *value;
    void (*handler)(void*);
};

typedef void (*handler_t)(void*);

struct trie *init_trie(void);
void insert_to_trie(struct trie *t, char *c, void (*handler)(void*));
handler_t insert_kv_pair_to_trie(struct trie *t, char *key, char *value);
struct trie_node *traverse_to_child_char(char c, struct trie_node *n);
uint8_t iterate_key_value_pairs(struct trie *t, struct trie_node **reentrant, char **key, char **value);

#endif
