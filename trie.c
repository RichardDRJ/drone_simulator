#include "trie.h"
#include <stdlib.h>
#include <stdio.h>

struct trie *init_trie(void)
{
    return calloc(sizeof(struct trie), 1);
}

void insert_to_trie(struct trie *t, char *c, void (*handler)(int))
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
            n = n->centre;
        }
        else if(*c > n->c)
        {
            if(!n->right)
                n->right = calloc(sizeof(struct trie_node), 1);
            n = n->right;
        }
        else if(*c < n->c)
        {
            if(!n->left)
                n->left = calloc(sizeof(struct trie_node), 1);
            n = n->left;
        }
        else if(*c == n->c)
        {
            if(!n->centre)
                n->centre = calloc(sizeof(struct trie_node), 1);
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
