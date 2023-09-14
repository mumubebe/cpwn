#include <assert.h>
#include "../includes/cpwn.h"

int main()
{
    Elf *e = ELF64("./tests/build/cpwn");
    printf("ELF size: %d\n", (int)e->size);
    str *r = pwn_elf64_read(e, 0, 4);
    str_print(r);
    printf("%s\n", e->endian);
    printf("%s\n", e->arch);
    printf("0x%lx\n", (uint64_t)symbols(e, "pwn_sendline"));
}
