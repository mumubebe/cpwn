
#ifndef TUBE_H
#define TUBE_H

#include "pstr.h"

enum Tubetype {REMOTE_TUBE, PROCESS_TUBE};


typedef struct pidNode {
    pid_t pid;
    struct pidNode* next;
    struct pidNode* prev;
} pidNode;


typedef struct Tube { 
    pstr* buffer;
    int fd_in;
    int fd_out;
    enum Tubetype type;
    pidNode* pid_node;
} Tube;

int pwn_sendline(Tube* tb, pstr* ps);
int pwn_send(Tube* tb, pstr* ps);
void fillbuffer(Tube *tb, size_t n, float timeout);
pstr* pwn_recv(Tube* tb, size_t n, float timeout);
pstr* pwn_recvuntil(Tube *tb, pstr* pattern, int timeout);

#endif