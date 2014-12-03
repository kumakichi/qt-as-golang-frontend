#ifndef MSG_RING_H
#define MSG_RING_H

#include <QMutex>

typedef struct _ipc_ring {
    struct _ipc_ring_node *head;
    struct _ipc_ring_node *read;
    struct _ipc_ring_node *write;
    QMutex mutex;
} ipc_ring;

typedef struct _ipc_ring_node {
    struct _ipc_ring_node *next;
    int len;
    char *data;
} ipc_ring_node;

ipc_ring *ring_create(int num);
char *ring_get(ipc_ring *r, int *read_len);
void ring_write(ipc_ring *r, char *data, int len);
void ring_delete(ipc_ring **pr);

#endif // MSG_RING_H
