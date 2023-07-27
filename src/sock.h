
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pstr.h"
#include "tube.h"

#ifndef SOCK_H
#define SOCK_H 

typedef struct Sock {
    char* host;
    int port;
    Tube tube;
} Sock;

typedef struct Sock Remote;

#endif