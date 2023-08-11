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
#include "log.h"


void load_64header(Elf64* elf); 

int open_elf(char* path) {
    int fd = open(path, O_RDONLY);

    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    LOG_DEBUG("File open: '%s' (fd: %d)", path, fd);
    return fd;
}

Elf64* ELF64(char *path) {
    Elf64 *elf = malloc(sizeof(Elf64));

    int fd = open_elf(path);

    struct stat sbuf;

    if (fstat(fd, &sbuf) == -1) {
        exit(EXIT_FAILURE);
    }

    elf->size = sbuf.st_size;

    elf->addr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (strncmp(elf->addr+1, "ELF", 3)) {
        LOG_INFO("File is not an ELF\n");
        exit(EXIT_FAILURE);
    }

    Elf64_Ehdr* header = (Elf64_Ehdr*) elf->addr; 
    elf->elf_header = header;

    load_64header(elf);

    return elf;    
}

void load_64header(Elf64* elf) {
    Elf64_Ehdr* header = elf->elf_header;

    elf->endian = header->e_ident[EI_DATA] == ELFDATA2LSB ? "little" : "big";
    
    // set architecture
    uint16_t e_machine = header->e_machine;
    switch(e_machine) {
        case EM_X86_64:
            elf->arch = "amd64";
            break;
        case EM_386:
            elf->arch = "i386";
            break;
        case EM_ARM:
            elf->arch = "arm";
            break;
        case EM_AARCH64:
            elf->arch = "aarch64";
            break;
        case EM_MIPS:
            elf->arch = "mips";
            break;
        case EM_PPC:
            elf->arch = "powerpc";
            break;
        case EM_PPC64:
            elf->arch = "powerpc64";
            break;
        case EM_SPARC32PLUS:
            elf->arch = "sparc";
            break;
        case EM_SPARCV9:
            elf->arch = "sparc64";
            break;
        case EM_IA_64:
            elf->arch = "ia64";
            break;
        default:
            elf->arch = "unknown";
            break;
    } 
}




/**
 * Read data from the specified virtual address
 * 
 * Returns str*
*/
str* pwn_elf64_read(Elf64* e, uint64_t addr, uint64_t count) {
    str* p = str_new_rawempty(count);
    memcpy(p->buf, (char*)(e->addr+addr), count);
    return p;
}


