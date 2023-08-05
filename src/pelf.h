#include <sys/types.h>
#include <stdint.h>
#include <elf.h>
#include "str.h"

typedef struct Eb32 {

} Eb32;

typedef struct Eb64 {
    Elf64_Ehdr* eh;
    Elf64_Shdr* sh;
    char* section_names;
} Eb64;


typedef struct Elf {
    void*       elfobj;
    void*       addr;
    char*       section_names;
    char*       endian;
    uint64_t    entry;
    uint64_t    size;
} Elf;

Elf*    ELF(char* path);
str*   pwn_elf_read(Elf* e, uint64_t addr, uint64_t count);