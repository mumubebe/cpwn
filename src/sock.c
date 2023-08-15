#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include "sock.h"
#include "tube.h"
#include "str.h"
#include "log.h"

int remote_can_recv_raw(Tube *tb, float timeout);

Tube *pwn_remote(char *host, int port)
{
    int fd;
    struct sockaddr_in server_addr;

    LOG_INFO("Opening connection to %s on port %d", host, port);
    LOG_INFO("Opening connection to %s on port %d: Trying %s", host, port, host);
    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    LOG_INFO("Opening connection to %s on port %d: Done", host, port);

    Tube *tube = malloc(sizeof(Tube));

    /* init empty buffer to tube struct */
    tube->buffer = str_new("");

    /* Tube handles both out and in fds (stdin/stdout), socket are same */
    tube->fd_out = fd;
    tube->fd_in = fd;
    tube->type = REMOTE_TUBE;

    return tube;
}

int remote_can_recv_raw(Tube *tb, float timeout)
{
    /* Setup poll */
    struct pollfd fds[1];

    fds[0].fd = tb->fd_out;
    fds[0].events = POLLIN;

    // poll takes timeout as millisecs
    int ret = poll(fds, 1, timeout * 1000.0f);

    if (ret == -1)
    {
        perror("poll");
        return -1;
    }

    if (!ret)
    {
        return 0;
    }

    if (fds[0].revents & POLLIN)
    {
        return 1;
    }
}

str *remote_recv_raw(Sock *s)
{
}