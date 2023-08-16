#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>
#include <paths.h>
#include <signal.h>
#include "str.h"
#include "process.h"
#include "tube.h"
#include <time.h>

#define while_timer(n) for (time_t end = time(NULL) + n; time(NULL) < end;)
#define timeleft end - time(NULL)

int pwn_sendline(Tube *tb, str *ps)
{
    str *ps2 = str_cpy(ps);
    str_cat_raw(ps2, "\n", 1);
    int length = pwn_send(tb, ps2);
    str_free(ps2);
    return length;
}

int pwn_send(Tube *tb, str *ps)
{
    int length = write(tb->fd_in, ps->buf, ps->length);
    if (length == -1)
    {
        perror("write");
    }
    return length;
}

str *pwn_recv(Tube *tb, size_t n, float timeout)
{
    if ((tb->buffer->length > n))
    {
        return str_popleft(tb->buffer, n);
    }

    fillbuffer(tb, n, timeout);

    return str_popleft(tb->buffer, n);
}

str *pwn_recvline(Tube *tb, int timeout)
{
    str *nl = str_new("\n");
    str *res = pwn_recvuntil(tb, nl, timeout);
    str_free(nl);
    return res;
}

void fillbuffer(Tube *tb, size_t n, float timeout)
{
    str *recv;
    if (tb->type == PROCESS_TUBE)
    {
        recv = process_recv_raw(tb, n, timeout);
    }

    if (recv->length > 0)
    {
        tb->buffer = str_cat_str(tb->buffer, recv);
        str_free(recv);
    }
}

/**
 *   Receive data until pattern is encountered.

    If the request is not satisfied before int timeout seconds(!) pass,
    all data is buffered and an empty str ("") is returned.
*/
str *pwn_recvuntil(Tube *tb, str *pattern, int timeout)
{
    str *readbuf = str_new("");
    str *curr = NULL;
    str *rest = NULL;
    int index;
    while_timer(timeout)
    {
        if (curr != NULL)
        {
            str_free(rest);

            rest = str_popright(curr, str_len(pattern));
            str_cat_str(readbuf, curr);
        }

        curr = pwn_recv(tb, 1024, timeleft);

        if (rest != NULL)
        {
            // fix this with a left concat later (implement in str).. :)
            str_cat_str(rest, curr);
            curr = rest;
        }
        if ((index = str_find(curr, pattern)) != -1)
        {
            // Get str up to found index
            str *uptoindex = str_popleft(curr, index + str_len(pattern));
            str_cat_str(readbuf, uptoindex);
            // Put rest to buffer
            str_cat_str(tb->buffer, curr);
            str_free(uptoindex);
            return readbuf;
        }
    }
    str_cat_str(tb->buffer, readbuf);
    str_free(readbuf);
    str_free(curr);
    return str_new("");
}

void pwn_shutdown(Tube *tb, char *direction)
{
    if (strcmp("send", direction) == 0)
    {
        close(tb->fd_in);
    }

    if (strcmp("recv", direction) == 0)
    {
        close(tb->fd_out);
    }
}
