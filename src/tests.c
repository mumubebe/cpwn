#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "pstr.h"
#include "process.h"
#include <sys/wait.h>
#include <unistd.h>
#include "sock.h"
#include "utils.h"
#include "log.h"

void remote_test() {
    LOG_DEBUG("--- RUNNING REMOTE TEST ---");
    Tube* t1 = pwn_process("nc -lvnp 4441");
    sleep(5);
    Tube* r1 = pwn_remote("127.0.0.1", 4441);
    pwn_sendline(r1, pstr_new("Hello"));
    usleep(10);

    LOG_DEBUG("remote_test: success");

}

void process_test() {
    LOG_DEBUG("--- RUNNING PROCESS TEST ---");
    Tube* t1 = pwn_process("nc -lvnp 4441");
    sleep(2);
    Tube* t2 = pwn_process("nc localhost 4441");
    
    pstr* r;
    pstr* s;
    pstr* t;

    // recvuntil
    s = pstr_new("123ABC");
    pwn_sendline(t2, s);
    usleep(10);

    t = pstr_new("123");
    pwn_recvuntil(t1, t, 2);
    pstr_free(t);

    r = pwn_recv(t1, 100, 1);
    t = pstr_new("ABC\n");
    assert(pstr_cmp(t, r) == 0); 
    pstr_free(r);
    pstr_free(t);
    pstr_free(s);

    // raw send/recv
    s = pstr_new_raw("\x01\x02\x03\x04", 4); 
    pwn_send(t2, s);
    usleep(100);
    
    r = pwn_recv(t1, 100, 1);
    assert(pstr_cmp(s, r) == 0);
    pstr_free(r);
    pstr_free(s);

    kill_processes();
    LOG_DEBUG("process_test: success");
}

void pstr_test() {
    LOG_DEBUG("--- RUNNING PSTR TEST ---");
    pstr* x;
    pstr* y;

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

    LOG_DEBUG("pstr_test: success");
    
}

void utils_test() {
    LOG_DEBUG("--- RUNNING UTILS TEST ---");
    pstr* x;
    pstr* y;

    x = pwn_p64(1, "little");
    y = pstr_new_raw("\x01\x00\x00\x00\x00\x00\x00\x00", 8);
    assert(pstr_cmp(x, y) == 0);
    pstr_free(x);
    pstr_free(y);

    x = pwn_p64(0xffffffffffffffff, "little");
    y = pstr_new_raw("\xff\xff\xff\xff\xff\xff\xff\xff", 8);
    assert(pstr_cmp(x, y) == 0);
    pstr_free(x);
    pstr_free(y);

    x = pwn_p64(1, "big");
    y = pstr_new_raw("\x00\x00\x00\x00\x00\x00\x00\x01", 8);
    assert(pstr_cmp(x, y) == 0);
    pstr_free(x);
    pstr_free(y);
    
    x = pwn_p32(1, "little");
    y = pstr_new_raw("\x01\x00\x00\x00", 4);
    assert(pstr_cmp(x, y) == 0);
    pstr_free(x);
    pstr_free(y);
    
    LOG_DEBUG("utils_test: success");

}