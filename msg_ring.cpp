#include "msg_ring.h"
#include "parseconf.h"
#include <stdlib.h>
#include <string.h>

ipc_ring *ring_create(int num)
{
    ipc_ring *r;
    ipc_ring_node *node;
    int max_data_size = MaxPktSize();
    int node_size = sizeof(*node);

    r = (ipc_ring *)calloc(1, sizeof(ipc_ring));
    r->head = (ipc_ring_node *)calloc(1, node_size);
    r->head->data = (char *)calloc(1, max_data_size);
    r->read = r->write = r->head;

    node = r->head;
    for(num -=1;num>0;num--) {
        node->next = (ipc_ring_node *)calloc(1, node_size);
        node->next->data = (char *)calloc(1, max_data_size);
        node=node->next;
    }
    node->next = r->head; //ring

    return r;
}

int check_pointer(void *ptr)
{
    if(NULL == ptr)
        return -1;
    else
        return 0;
}

char *ring_get(ipc_ring *r, int *read_len)
{
    char *data = NULL;

    *read_len = 0;
    r->mutex.lock();
    if(r->read != r->write) {
        data = r->read->data;
        *read_len = r->read->len;
        r->read = r->read->next;
    }
    r->mutex.unlock();
    return data;
}

void ring_write(ipc_ring *r, char *data, int len)
{
    r->mutex.lock();
    memcpy(r->write->data, data, len);
    r->write->len = len;
    r->write = r->write->next;
    r->mutex.unlock();
}

void ring_delete(ipc_ring **pr)
{
    ipc_ring_node *r;
    ipc_ring_node **cur = &(*pr)->head;

    for(;*cur;) {
        r= *cur;
        *cur = r->next;
        free(r->data);
        free(r);
    }
}
