#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include "pelf.h"
#include "str.h"


char* get_section_name_at_index(Eb64 *eb, int index);

Elf* ELF(char *path) {
    int fd;
    Elf *eobj = malloc(sizeof(Elf));

    fd = open(path, O_RDONLY);

    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct stat sbuf;

    if (fstat(fd, &sbuf) == -1) {
        exit(EXIT_FAILURE);
    }
    eobj->size = sbuf.st_size;
    eobj->addr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    
    if (strncmp(eobj->addr+1, "ELF", 3)) {
        printf("File is not an ELF\n");
        exit(EXIT_FAILURE);
    }

    Eb64* eb64 = malloc(sizeof(Eb64));

    eobj->elfobj = eb64;
    
}

/**
 * Read data from the specified virtual address
 * 
 * Returns str*
*/
str* pwn_elf_read(Elf* e, uint64_t addr, uint64_t count) {
    str* p = str_new_rawempty(count);
    memcpy(p->buf, (char*)(e->addr+addr), count);
    return p;
}

char* get_section_name_at_index(Eb64 *eb, int index) {
    return &eb->section_names[eb->sh[index].sh_name]; 
}

