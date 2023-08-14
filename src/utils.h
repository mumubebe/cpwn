#include "str.h"
#include <sys/types.h>


#ifndef UTILS_H 
#define UTILS_H 

extern str*   pwn_p64(u_int64_t value, char* endian); 
extern str*   pwn_p32(u_int64_t value, char* endian); 

#endif