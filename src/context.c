#include "context.h"
#include "log.h"

void init_context() __attribute__((constructor));


struct_context pwn_context;

void init_context() {
    LOG_DEBUG("Init context");

    pwn_context.arch = "i386";
    pwn_context.endian = "little";
    pwn_context.log_level = "info";
    pwn_context.sign = "unsigned";
}