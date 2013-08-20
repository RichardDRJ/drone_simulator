#include "data_structures/linked_list.h"
#include <pthread.h>

static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

struct list_elem *list_pop(struct linked_list *l)
{
    pthread_mutex_lock(&list_mutex);
    struct list_elem *e = l->head;
    if(l->head == l->tail)
        l->tail = l->head = NULL;
    else
        l->head = e->next;
    
    pthread_mutex_unlock(&list_mutex);
    
    return e;
}

void list_add(struct linked_list *l, void *data, size_t length)
{
    pthread_mutex_lock(&list_mutex);

    struct list_elem *e = calloc(sizeof(struct list_elem), 1);
    if(l->tail)
        l->tail->next = e;
    else
        l->head = e;
    l->tail = e;
    e->data = data;
    
    pthread_mutex_unlock(&list_mutex);
}
