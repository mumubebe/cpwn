#include <sys/types.h>
#include <stdint.h>
#include <elf.h>
#include "str.h"



typedef struct Elf64 {
    Elf64_Ehdr* elf_header;
    Elf64_Shdr* section_header;
    void*       addr;
    char*       section_names;
    char*       endian;
    char*       arch;
    uint64_t    entry;
    uint64_t    size;
} Elf64;

Elf64*    ELF64(char* path);
str*   pwn_elf64_read(Elf64* e, uint64_t addr, uint64_t count);

typedef Elf64 Elf;
