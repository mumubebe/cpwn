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
char* section_name_at_index(Elf64 *elf, int index);
int index_by_section_name(Elf64 *elf, char* name);


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

    elf->path = path;
    elf->size = sbuf.st_size;

    elf->addr = (uint64_t)mmap(NULL, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (strncmp(elf->addr+1, "ELF", 3)) {
        LOG_INFO("File is not an ELF\n");
        exit(EXIT_FAILURE);
    }

    Elf64_Ehdr* header = (Elf64_Ehdr*) elf->addr; 
    elf->elf_header = header;


    Elf64_Shdr* section_header = (Elf64_Shdr*) ((char*)elf->addr + header->e_shoff);
    elf->section_header = section_header;

    // Set section header string table
    char *section_name = (char*)elf->addr + elf->section_header[elf->elf_header->e_shstrndx].sh_offset;
    elf->section_header_string_table = section_name;

    printf("%s\n", section_name_at_index(elf, 12));

    Elf64_Shdr* plt = &section_header[index_by_section_name(elf, ".plt")];
    printf("%s\n", ((char*)elf->section_header_string_table + plt->sh_name));
    printf("%d\n", (int)plt->sh_offset);

    str* pltstr = pwn_elf64_read(elf, plt->sh_offset, plt->sh_size);
    str_print(pltstr);
    


    load_64header(elf);

    return elf;    
}

void checksec64(Elf64* elf) { }

void checksec(char* path, char* arch, int btype, char* endian) {
    LOG_INFO(
        "'%s'\n\tArch:%s-%s-%s", 
        path, arch, btype, endian);

}

void load_64header(Elf64* elf) {
    Elf64_Ehdr* header = elf->elf_header;

    elf->endian = header->e_ident[EI_DATA] == ELFDATA2LSB ? "little" : "big";
    
    // relabel only a handful architectures for now..
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




int index_by_section_name(Elf64 *elf, char* name) {
    for (int i=0; i < elf->elf_header->e_shnum; i++) {
        if (strcmp(section_name_at_index(elf, i), name) == 0) {
            return i;
        }
    }
    return -1;
}

char* section_name_at_index(Elf64 *elf, int index) {
    return &elf->section_header_string_table[elf->section_header[index].sh_name]; 
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


