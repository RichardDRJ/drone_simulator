#include "data_structures/linked_list.h"

struct list_elem *list_pop(struct linked_list *l)
{
    struct list_elem *e = l->head;
    if(l->head == l->tail)
        l->tail = l->head = NULL;
    else
        l->head = e->next;
    return e;
}

void list_add(struct linked_list *l, void *data, size_t length)
{
    struct list_elem *e = calloc(sizeof(struct list_elem), 1);
    if(l->tail)
        l->tail->next = e;
    else
        l->head = e;
    l->tail = e;
    e->data = data;
}
