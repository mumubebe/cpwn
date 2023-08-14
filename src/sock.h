
#include "str.h"
#include "tube.h"

#ifndef SOCK_H
#define SOCK_H 

typedef struct Sock {
    char* host;
    int port;
    Tube* tube;
} Sock;


extern Tube* pwn_remote(char *host, int port);
extern void remote_init(Sock *sock);

#endif