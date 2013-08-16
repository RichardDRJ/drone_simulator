#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

struct linked_list
{
    struct list_elem *head;
    struct list_elem *tail;
};

struct list_elem
{
    void *data;
    size_t length;
    struct list_elem *next;
};

struct list_elem *list_pop(struct linked_list *l);
void list_add(struct linked_list *l, void *data, size_t length);

#endif
