#include "src/process.h"
#include "src/pstr.h"
#include <assert.h>


void pstr_test();
void process_test();

int main() {
    Process p = {
        .cmd = "/usr/bin/cat"
    };

    pstr_test();

    init_process(&p);

    pstr* payload = pstr_new("2");
    printf("%s\n", process_recv_raw(&p, 100, 50));
    process_sendline(&p, payload);
    process_sendline(&p, payload);
    printf("%s\n", process_recv_raw(&p, 100, 50));
}

void process_test() {

}


void pstr_test() {
    pstr* str1 = pstr_new("hello");
    assert(strlen(str1->buf) == 5);
    assert(pstr_len(str1) == 5);

    pstr* bstr1 = pstr_new_raw("hello\x00\x04there", 12);
    assert(strlen(bstr1->buf) == 5);
    assert(pstr_len(bstr1) == 12);

    pstr* bstr2 = pstr_cat(bstr1, "!!!");
    assert(strlen(bstr2->buf) == 5);
    assert(pstr_len(bstr2) == 15);

    pstr* popstr1 = pstr_new("ABCDEF");
    pstr* substr1 = pstr_pop(popstr1, 2);
    assert(pstr_len(popstr1) == 4 );
    assert(pstr_len(substr1) == 2 );

    printf("%s\n", substr1->buf);
    printf("%s\n", popstr1->buf);

    pstr* popstr2 = pstr_new_raw("ABCD\x04\x00\EF", 8);
    pstr* substr2 = pstr_pop(popstr2, 4);
    assert(pstr_len(substr2) == 4);



}