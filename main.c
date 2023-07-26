#include "src/process.h"
#include "src/pstr.h"
#include "src/utils.h"
#include <assert.h>


void pstr_test();
void process_test();

int main() {
    Process p = {
        .cmd = "nc -lvnp 4443"
    };
    init_process(&p);
    
    pstr* r = process_readuntil(&p, pstr_new_raw("\x04\x05", 2), 20);
    pstr_print(r);

    pstr_print(process_recv(&p, 10, 5));


    pstr_print(p64(1337, "l2ittle"));
}

void process_test() {

}


void pstr_test() {
    pstr* x;
    pstr* y;
    pstr* z;

    x = pstr_new("hello");
    assert(strlen(x->buf) == 5);
    assert(pstr_len(x) == 5);
    pstr_free(x);

    x = pstr_new_raw("hello\x00\x04there", 12);
    assert(strlen(x->buf) == 5);
    assert(pstr_len(x) == 12);
    pstr_cat(x, "!!!");
    assert(strlen(x->buf) == 5);
    assert(pstr_len(x) == 15);
    pstr_free(x);

    x = pstr_new("");
    y = pstr_new("ABC");
    pstr_cat_pstr(x, y);
    assert(pstr_len(x) == 3);
    pstr_free(x);
    pstr_free(y);

    x = pstr_new("ABCDEF");
    y = pstr_popright(x, 2);
    assert(pstr_len(x) == 4 );
    assert(pstr_len(y) == 2 );
    pstr_free(x);
    pstr_free(y);

    x = pstr_new("ABCDEF");
    y = pstr_popleft(x, 2);
    assert(pstr_len(x) == 4 );
    assert(pstr_len(y) == 2 );
    pstr_free(x);
    pstr_free(y);

    x = pstr_new_raw("ABCD\x04\x00\EF", 8);
    y = pstr_popright(x, 4);
    assert(pstr_len(y) == 4);
    pstr_free(x);
    pstr_free(y);


    x = pstr_new("Hello");
    y = pstr_new("ll");
    int index = pstr_find(x, y);
    assert(index == 2);
    pstr_free(x);
    pstr_free(y);

    
}