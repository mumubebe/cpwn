#include "pstr.h"
#include <sys/types.h>


#ifndef UTILS_H 
#define UTILS_H 

pstr* pwn_p64(u_int64_t value, char* endian); 
pstr* pwn_p32(u_int64_t value, char* endian); 

#endif