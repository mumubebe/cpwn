
#ifndef TUBE_H
#define TUBE_H

#include "str.h"

enum Tubetype {REMOTE_TUBE, PROCESS_TUBE};

typedef struct pidNode {
    pid_t pid;
    struct pidNode* next;
    struct pidNode* prev;
} pidNode;


typedef struct Tube { 
    str* buffer;
    int fd_in;
    int fd_out;
    enum Tubetype type;
    pidNode* pid_node;
} Tube;

int     pwn_sendline(Tube* tb, str* ps);
int     pwn_send(Tube* tb, str* ps);
void    fillbuffer(Tube *tb, size_t n, float timeout);
str*   pwn_recv(Tube* tb, size_t n, float timeout);
str*   pwn_recvuntil(Tube *tb, str* pattern, int timeout);
void    pwn_shutdown(Tube* tb, char* direction);

#endif