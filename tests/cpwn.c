#include <assert.h>
#include "../includes/cpwn.h"

int main()
{
    
    printf("Arch: %s\n", pwn_context.arch);
    pwn_context.arch = "amd64";
    printf("Arch: %s\n", pwn_context.arch);
    which_bintools("objdump");
}
