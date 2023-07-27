#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "pstr.h"
#include "process.h"
#include <sys/wait.h>
#include <unistd.h>
#include "utils.h"


void process_test() {
    Process p1 = {
        .cmd = "nc -lvnp 4443"
    };
    init_process(&p1);
    
    Process p2 = {
        .cmd = "nc localhost 4443"
    };
    pstr* r;
    pstr* s;
    pstr* t;
    init_process(&p2);

    // recvuntil
    s = pstr_new("123ABC");
    process_sendline(&p2, s);
    usleep(10);

    t = pstr_new("123");
    process_recvuntil(&p1, t, 2);
    pstr_free(t);

    r = process_recv(&p1, 100, 1);
    t = pstr_new("ABC\n");
    assert(pstr_cmp(t, r) == 0); 
    pstr_free(r);
    pstr_free(t);
    pstr_free(s);

    // raw send/recv
    s = pstr_new_raw("\x01\x02\x03\x04", 4); 
    process_send(&p2, s);
    usleep(10);
    
    r = process_recv(&p1, 100, 1);
    assert(pstr_cmp(s, r) == 0);
    pstr_free(r);
    pstr_free(s);

    printf("process_test: success\n");
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

    printf("pstr_test: success\n");
    
}

void utils_test() {
    pstr* x;
    pstr* y;

    x = p64(1, "little");
    y = pstr_new_raw("\x01\x00\x00\x00\x00\x00\x00\x00", 8);
    assert(pstr_cmp(x, y) == 0);
    pstr_free(x);
    pstr_free(y);

    x = p64(0xffffffffffffffff, "little");
    y = pstr_new_raw("\xff\xff\xff\xff\xff\xff\xff\xff", 8);
    assert(pstr_cmp(x, y) == 0);
    pstr_free(x);
    pstr_free(y);

    x = p64(1, "big");
    y = pstr_new_raw("\x00\x00\x00\x00\x00\x00\x00\x01", 8);
    assert(pstr_cmp(x, y) == 0);
    pstr_free(x);
    pstr_free(y);
    
    x = p32(1, "little");
    y = pstr_new_raw("\x01\x00\x00\x00", 4);
    assert(pstr_cmp(x, y) == 0);
    pstr_free(x);
    pstr_free(y);
    
    printf("utils_test: success\n");

}