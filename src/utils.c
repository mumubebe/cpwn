#include "pstr.h"
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define LENDIAN 0
#define BENDIAN 1 


void to_bytestr(void* value, size_t size, char* output, int endian);
int validate_endian(char* endian);

pstr* pwn_p64(u_int64_t value, char *endian) {
    char out64[8];
    to_bytestr(&value, sizeof(out64), out64, validate_endian(endian));
    return pstr_new_raw(out64, sizeof(out64));
}

pstr* pwn_p32(u_int32_t value, char *endian) {
    char out32[4];
    to_bytestr(&value, sizeof(out32), out32, validate_endian(endian));
    return pstr_new_raw(out32, sizeof(out32));
}

int validate_endian(char* endian) {
    if (strcmp(endian, "little") == 0) {
        return LENDIAN;
    }
    else if (strcmp(endian, "big") == 0) {
        return BENDIAN;
    } else {
        printf("error: only 'small' or 'big' are valid params for endian\n");
        exit(EXIT_FAILURE);
    }
}



void to_bytestr(void* value, size_t size, char* output, int endian) {
    if(endian == LENDIAN) {
        for (size_t i = 0; i < size; i++) {
            output[i] = *((char*)value + i) & 0xFF;
        }
    } else {
        for (size_t i = 0; i < size; i++) {
            output[size - 1 - i] = *((char*)value + i) & 0xFF;
        }
    }
}