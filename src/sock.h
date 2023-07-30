
#include "pstr.h"
#include "tube.h"

#ifndef SOCK_H
#define SOCK_H 

typedef struct Sock {
    char* host;
    int port;
    Tube* tube;
} Sock;


Tube* pwn_remote(char *host, int port);
void remote_init(Sock *sock);

#endif