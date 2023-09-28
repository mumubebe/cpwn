#ifndef CONTEXT_H
#define CONTEXT_H

typedef struct struct_context {
    char* arch;
    char* endian;
    char* binary;
    char* log_file;
    char* log_level;
    char* sign;
    char* timeout;
} struct_context;

extern struct_context pwn_context;
#endif