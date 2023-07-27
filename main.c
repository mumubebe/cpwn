#include "src/process.h"
#include "src/pstr.h"
#include "src/utils.h"
#include "src/tests.h"
#include <assert.h>

int main() {
    pstr_test();
    Process p = {
        .cmd = "nc -lvnp 4443"
    };
    init_process(&p);
    
    pstr* r = process_readuntil(&p, pstr_new_raw("\x04\x05", 2), 20);
    //process_sendline(&p, pstr_new("Hello"));
    pstr_print(r);
    pstr_print(process_recv(&p, 100, 2));
}


