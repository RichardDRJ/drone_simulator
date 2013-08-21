#include "trie.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define KEY_LENGTH 100
#define VALUE_LENGTH 100

struct trie *init_trie(void)
{
    return calloc(sizeof(struct trie), 1);
}

uint8_t iterate_key_value_pairs(struct trie *t, struct trie_node **reentrant, char **key, char **value)
{
    struct trie_node *n;
    if(*reentrant)
        n = *reentrant;
    else
        n = t->root;

    while(!n->key)
    {
        switch(n->next_traverse++)
        {
            case 0:
                if(n->left)
                {
                    n = n->left;
                    break;
                }
            case 1:
                if(n->centre)
                {
                    n = n->centre;
                    break;
                }
            case 2:
                if(n->right)
                {
                    n = n->right;
                    break;
                }
            case 3:
                n->next_traverse = 0;

                if(n->parent)
                    n = n->parent;
                else
                    return 0;
        }
    }

    *value = n->value;
    *key = n->key;
    return 1;
}

handler_t insert_kv_pair_to_trie(struct trie *t, char *key, char *value)
{
    struct trie_node *n = t->root;
    char *c = key;

    if(!n)
    {
        n = calloc(sizeof(struct trie_node), 1);
        t->root = n;
    }

    while(*c)
    {
        if(n->c == '\0')
        {
            n->c = *c++;
            n->centre = calloc(sizeof(struct trie_node), 1);
            n->centre->parent = n;
            n = n->centre;
        }
        else if(*c > n->c)
        {
            if(!n->right)
            {
                n->right = calloc(sizeof(struct trie_node), 1);
                n->right->parent = n;
            }
            n = n->right;
        }
        else if(*c < n->c)
        {
            if(!n->left)
            {
                n->left = calloc(sizeof(struct trie_node), 1);
                n->left->parent = n;
            }
            n = n->left;
        }
        else if(*c == n->c)
        {
            if(!n->centre)
            {
                n->centre = calloc(sizeof(struct trie_node), 1);
                n->centre->parent = n;
            }
            n = n->centre;
            ++c;
        }
    }

    if(!n->value)
        n->value = malloc(sizeof(char) * VALUE_LENGTH);
    if(!n->key)
        n->key = malloc(sizeof(char) * KEY_LENGTH);

    strcpy(n->value, value);
    strcpy(n->key, key);

    return n->handler;
}

void insert_to_trie(struct trie *t, char *c, void (*handler)(void*))
{
    struct trie_node *n = t->root;
    char *key = c;

    if(!n)
    {
        n = calloc(sizeof(struct trie_node), 1);
        t->root = n;
    }

    while(*c)
    {
        if(n->c == '\0')
        {
            n->c = *c++;
            n->centre = calloc(sizeof(struct trie_node), 1);
            n->centre->parent = n;
            n = n->centre;
        }
        else if(*c > n->c)
        {
            if(!n->right)
            {
                n->right = calloc(sizeof(struct trie_node), 1);
                n->right->parent = n;
            }
            n = n->right;
        }
        else if(*c < n->c)
        {
            if(!n->left)
            {
                n->left = calloc(sizeof(struct trie_node), 1);
                n->left->parent = n;
            }
            n = n->left;
        }
        else if(*c == n->c)
        {
            if(!n->centre)
            {
                n->centre = calloc(sizeof(struct trie_node), 1);
                n->centre->parent = n;
            }
            n = n->centre;
            ++c;
        }
    }

    n->handler = handler;
    n->key = key;
}

struct trie_node *traverse_to_child_char(char c, struct trie_node *n)
{
    while(n)
    {
        if(c > n->c)
            n = n->right;
        else if(c < n->c)
            n = n->left;
        else if(c == n->c)
            break;
    }

    if(n)
        return n->centre;
    return 0;
}
