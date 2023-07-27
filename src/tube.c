#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>
#include <paths.h>
#include <signal.h>
#include "pstr.h"
#include "process.h"
#include "tube.h"
#include <time.h>


#define while_timer(n) for(time_t end = time(NULL) + n; time(NULL) < end; )
#define timeleft end-time(NULL)

int sendline(Tube* tb, pstr* ps) {
    pstr* ps2 = pstr_cat_raw(ps, "\n", 1);
    send(tb, ps2);
}

int send(Tube* tb, pstr* ps) {
    int length = write(tb->fd_in[WRITE_END], ps->buf, ps->length);
    if (length < 0) {
        perror("write");
    }
    return length;
}


pstr* recv(Tube* tb, size_t n, float timeout, pstr* (*recv_raw)(Tube*, size_t, float)) {
    if ((tb->buffer->length > n)) {
        return pstr_popleft(tb->buffer, n);
    } 
    fillbuffer(tb, n, timeout, recv_raw);
    
    return pstr_popleft(tb->buffer, n);
}

void fillbuffer(Tube *tb, size_t n, float timeout, pstr* (*recv_raw)(Tube*, size_t, float)) {
    pstr* recv = recv_raw(tb, n, timeout);

    if (recv->length > 0) {
        tb->buffer = pstr_cat_pstr(tb->buffer, recv);
    }
}

/**
 *   Receive data until pattern is encountered.

    If the request is not satisfied before int timeout seconds(!) pass,
    all data is buffered and an empty pstr ("") is returned.
*/
pstr* readuntil(Tube *tb, pstr* pattern, int timeout, pstr* (*recv_raw)(Tube*, size_t, float)) {
    pstr* readbuf = pstr_new("");
    pstr* curr = NULL;
    pstr* rest = NULL;
    int index;
    while_timer(timeout) {
        if (curr != NULL) {
            pstr_free(rest);

            rest = pstr_popright(curr, pstr_len(pattern));
            pstr_cat_pstr(readbuf, curr);
        }

        curr = recv(tb, 1024, timeleft, recv_raw);

        if (rest != NULL) {
            // fix this with a left concat later (implement in pstr).. :)
            pstr_cat_pstr(rest, curr);
            curr = rest;
        }
        index = pstr_find(curr, pattern);
        if (index != -1) {
            // Get str up to found index
            pstr* uptoindex = pstr_popleft(curr, index+pstr_len(pattern)); 
            pstr_cat_pstr(readbuf, uptoindex);
            // Put rest to buffer
            pstr_cat_pstr(tb->buffer, curr);
            return readbuf;
        }
    }
    pstr_cat_pstr(tb->buffer, readbuf);
    pstr_free(readbuf);
    pstr_free(curr);
    return pstr_new("");
}
