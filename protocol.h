#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SYNC1 0xF8
#define SYNC2 0x8F
#define SYNC3 0x9F
#define SYNC4 0xF9

struct ipc_msg {
    unsigned char sync[4];
    char type;
    long int offset;
    bool lastPiece; // whether this message is the last piece or not
    long int len; //length of data element
    long int total_len;
    char data[];
};

#define HEADER_LEN sizeof(struct ipc_msg)

long int build_msg(char *shm, char type);
char parse_msg(char *shm, char *buff);

#define MSG_ASK_IMG 0x01
#define MSG_IMG 0x02
#define MSG_START 0x03
#define MSG_LBL 0x04

#endif // PROTOCOL_H
