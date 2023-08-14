#include <assert.h>
#include "../includes/cpwn.h"


int main() {
    //narnia0();
    //tests();
    Elf* e = ELF64("/usr/bin/cat");
    printf("ELF size: %d\n", e->size);
    str* r = pwn_elf64_read(e, 0, 4); 
    printf("%s\n", e->endian);
    printf("%s\n", e->arch);
    printf("\n");
}



