
#include "pstr.h"

#ifndef TUBE_H
#define TUBE_H

typedef struct Tube { 
    pstr* buffer;
    int fd_in[2];
    int fd_out[2];
} Tube;



int sendline(Tube* tb, pstr* ps);
int send(Tube* tb, pstr* ps);
void fillbuffer(Tube *tb, size_t n, float timeout, pstr* (*recv_raw)(Tube*, size_t, float));
pstr* recv(Tube* tb, size_t n, float timeout, pstr* (*recv_raw)(Tube*, size_t, float));
pstr* readuntil(Tube *tb, pstr* pattern, int timeout, pstr* (*recv_raw)(Tube*, size_t, float));

#endif