
#include "pstr.h"

#ifndef TUBE_H
#define TUBE_H

typedef struct Tube { 
    pstr* buffer;
    int fd_in;
    int fd_out;
} Tube;



int tube_sendline(Tube* tb, pstr* ps);
int tube_send(Tube* tb, pstr* ps);
void fillbuffer(Tube *tb, size_t n, float timeout, pstr* (*recv_raw)(Tube*, size_t, float));
pstr* tube_recv(Tube* tb, size_t n, float timeout, pstr* (*recv_raw)(Tube*, size_t, float));
pstr* tube_recvuntil(Tube *tb, pstr* pattern, int timeout, pstr* (*recv_raw)(Tube*, size_t, float));

#endif