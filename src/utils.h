#include "str.h"
#include <sys/types.h>


#ifndef UTILS_H 
#define UTILS_H 

str*   pwn_p64(u_int64_t value, char* endian); 
str*   pwn_p32(u_int64_t value, char* endian); 

#endif