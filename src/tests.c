#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "str.h"
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
    pwn_sendline(r1, str_new("Hello"));
    usleep(10);

    LOG_DEBUG("remote_test: success");

}

void process_test() {
    LOG_DEBUG("--- RUNNING PROCESS TEST ---");
    Tube* t1 = pwn_process("nc -lvnp 4441");
    sleep(2);
    Tube* t2 = pwn_process("nc localhost 4441");
    
    str* r;
    str* s;
    str* t;

    // recvuntil
    s = str_new("123ABC");
    pwn_sendline(t2, s);
    usleep(10);

    t = str_new("123");
    pwn_recvuntil(t1, t, 2);
    str_free(t);

    r = pwn_recv(t1, 100, 1);
    t = str_new("ABC\n");
    assert(str_cmp(t, r) == 0); 
    str_free(r);
    str_free(t);
    str_free(s);

    // raw send/recv
    s = str_new_raw("\x01\x02\x03\x04", 4); 
    pwn_send(t2, s);
    usleep(100);
    
    r = pwn_recv(t1, 100, 1);
    assert(str_cmp(s, r) == 0);
    str_free(r);
    str_free(s);

    kill_processes();
    LOG_DEBUG("process_test: success");
}

void str_test() {
    LOG_DEBUG("--- RUNNING str TEST ---");
    str* x;
    str* y;

    x = str_new("hello");
    assert(strlen(x->buf) == 5);
    assert(str_len(x) == 5);
    str_free(x);

    x = str_new_raw("hello\x00\x04there", 12);
    assert(strlen(x->buf) == 5);
    assert(str_len(x) == 12);
    str_cat(x, "!!!");
    assert(strlen(x->buf) == 5);
    assert(str_len(x) == 15);
    str_free(x);

    x = str_new("");
    y = str_new("ABC");
    str_cat_str(x, y);
    assert(str_len(x) == 3);
    str_free(x);
    str_free(y);

    x = str_new("ABCDEF");
    y = str_popright(x, 2);
    assert(str_len(x) == 4 );
    assert(str_len(y) == 2 );
    str_free(x);
    str_free(y);

    x = str_new("ABCDEF");
    y = str_popleft(x, 2);
    assert(str_len(x) == 4 );
    assert(str_len(y) == 2 );
    str_free(x);
    str_free(y);

    x = str_new_raw("ABCD\x04\x00\EF", 8);
    y = str_popright(x, 4);
    assert(str_len(y) == 4);
    str_free(x);
    str_free(y);


    x = str_new("Hello");
    y = str_new("ll");
    int index = str_find(x, y);
    assert(index == 2);
    str_free(x);
    str_free(y);

    LOG_DEBUG("str_test: success");
    
}

void utils_test() {
    LOG_DEBUG("--- RUNNING UTILS TEST ---");
    str* x;
    str* y;

    x = pwn_p64(1, "little");
    y = str_new_raw("\x01\x00\x00\x00\x00\x00\x00\x00", 8);
    assert(str_cmp(x, y) == 0);
    str_free(x);
    str_free(y);

    x = pwn_p64(0xffffffffffffffff, "little");
    y = str_new_raw("\xff\xff\xff\xff\xff\xff\xff\xff", 8);
    assert(str_cmp(x, y) == 0);
    str_free(x);
    str_free(y);

    x = pwn_p64(1, "big");
    y = str_new_raw("\x00\x00\x00\x00\x00\x00\x00\x01", 8);
    assert(str_cmp(x, y) == 0);
    str_free(x);
    str_free(y);
    
    x = pwn_p32(1, "little");
    y = str_new_raw("\x01\x00\x00\x00", 4);
    assert(str_cmp(x, y) == 0);
    str_free(x);
    str_free(y);
    
    LOG_DEBUG("utils_test: success");

}