#include <sys/types.h>
#include <stdint.h>
#include <elf.h>
#include "str.h"



typedef struct Elf64 {
    Elf64_Ehdr* elf_header;
    Elf64_Shdr* section_header;
    uint64_t       addr;

    char*       path;

    char*       section_names;

    /* Endianness of the file (e.g. 'big', 'little') */
    char*       endian;

    /* Architecture of the file (e.g. 'i386', 'arm').*/
    char*       arch;

    /* eshstrndx */
    char*       section_header_string_table;

    uint64_t    entry;
    uint64_t    size;
} Elf64;

extern Elf64*    ELF64(char* path);
extern str*   pwn_elf64_read(Elf64* e, uint64_t addr, uint64_t count);

typedef Elf64 Elf;
