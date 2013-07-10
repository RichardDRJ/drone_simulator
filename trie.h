#ifndef TRIE_H
#define TRIE_H

struct trie
{
    struct trie_node *root;
};

struct trie_node
{
    char c;
    struct trie_node *left;
    struct trie_node *centre;
    struct trie_node *right;
    char *key;
    void (*handler)(int);
};

struct trie *init_trie(void);
void insert_to_trie(struct trie *t, char *c, void (*handler)(int));
struct trie_node *traverse_to_char(char c, struct trie_node *n);

#endif
