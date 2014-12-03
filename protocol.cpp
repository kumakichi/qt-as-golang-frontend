#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
#include "protocol.h"

void header_sync(char *shm)
{
    shm[0] = SYNC1;
    shm[1] = SYNC2;
    shm[2] = SYNC3;
    shm[3] = SYNC4;
}

long int ask_for_img(char *shm, int type)
{
    struct ipc_msg * msg = (struct ipc_msg *)shm;

    msg->type = type;
    msg->offset = 0;
    msg->lastPiece = true;
    msg->len = 0;

    return msg->len;
}

long int build_msg(char *shm, char type)
{
    int len = -1;

    header_sync(shm);

    switch(type) {
    case MSG_ASK_IMG:
        len = ask_for_img(shm, type);
        break;
    default:
        break;
    }

    return len;
}

bool validate_sync(struct ipc_msg *msg)
{
    int i;
    unsigned char sync[] = {SYNC1,SYNC2,SYNC3,SYNC4};
    for(i=0;i<4;i++)
        if(msg->sync[i] != sync[i])
            return false;
    return true;
}

char parse_msg(char *shm, char *buff)
{
    struct ipc_msg * from = (struct ipc_msg *)buff;
    struct ipc_msg * to = (struct ipc_msg *)shm;

    if(!validate_sync(from))
        return -1;

    memcpy(shm, buff, HEADER_LEN); //copy header
    memcpy(to->data + from->offset, from->data, from->len); //copy real data

    return from->type;
}
