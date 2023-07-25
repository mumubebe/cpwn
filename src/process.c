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

pidNode* head = NULL;
pstr* buffer = NULL;

void fillbuffer(Process *p, size_t n, float timeout);


int process_sendline(Process *p, pstr *ps) {
    pstr* ps2 = pstr_cat_raw(ps, "\n", 1);
    process_send_raw(p, ps2);
}

int process_send_raw(Process *p, pstr* ps) {
    int length = write(p->fd_in[WRITE_END], ps->buf, ps->length);
    if (length < 0) {
        perror("write");
    }

    return length;
}

pstr* process_readuntil(Process *p, pstr* pattern, float timeout) {
    while(1) {

    }
}


pstr* process_recv(Process *p, size_t n, float timeout) {
    if ((p->buffer->length > n)) {
        return pstr_popleft(p->buffer, n);
    } 
    fillbuffer(p, n, timeout);
    
    return pstr_popleft(p->buffer, n);
}

void fillbuffer(Process *p, size_t n, float timeout) {
    pstr* recv = process_recv_raw(p, n, timeout);

    if (recv->length > 0) {
        p->buffer = pstr_cat_pstr(p->buffer, recv);
    }
}

/*
 * Recv number `n` bytes from process
 * 
 * if timeout or EOF is reached, return value points to an empty string.
*/
pstr* process_recv_raw(Process *p, size_t n, float timeout) {
    if (process_can_recv_raw(p, timeout)) {
        char* buf = malloc(sizeof(char) * n);
        
        if (!buf) {
            perror("malloc");
            return NULL;
        }
        size_t len;
        len = read(p->fd_out[READ_END], buf, n-1);
        if (len == -1) {
            perror("read");
            return pstr_new("");
        }

        if (len) {
            return pstr_new_raw(buf, len);
        }
    }
    return pstr_new("");
}



/**
 * Checks in a nonblocking way if we can read from process
 * 
 * returns `1` if we have data to read.
*/
int process_can_recv_raw(Process *p, float timeout) {
    /* Setup poll */
    struct pollfd fds[1];

    fds[0].fd = p->fd_out[READ_END];
    fds[0].events = POLLIN;

    int ret = poll(fds, 1, timeout);

    if (ret == -1) {
        perror("poll");
        return -1;
    }

    if (!ret) {
        return 0;
    }

    if (fds[0].revents & POLLIN) {
        return 1;
    }
}


int init_process(Process *p) {
    int fd_out[2];
    int fd_in[2];

    if (pipe(fd_out) == -1 || pipe(fd_in) == -1) {
        perror("pipe");
    }

    /* Stores file descriptors in current process struct */
    memcpy(p->fd_in, fd_in, sizeof(p->fd_in));;
    memcpy(p->fd_out, fd_out, sizeof(p->fd_out));;

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed:");
        return -1;
    } 
    else if (pid > 0) {
        /* Add forked process to a pidNode linked list to keep track of all forked processes */

        pidNode* pid_node = malloc(sizeof(pidNode));
        if (!pid_node) { perror("malloc"); return -1; }

        pid_node->pid = pid;
        pid_node->next = head;
        pid_node->prev = NULL;

        p->pid_node = pid_node;

        if (head != NULL) {
            head->prev = pid_node;
        }
        head = pid_node;


        /* init empty to process struct */
        p->buffer = pstr_new("");
        
        /* Register kill process atexit */
        atexit(kill_processes);
    } 
    else {
        /* Duplicate FD to FD2, closing FD2 and making it open on the same file */
        dup2(fd_in[READ_END], STDIN_FILENO);
        dup2(fd_out[WRITE_END], STDOUT_FILENO);

        
        char *args[] = {p->cmd, NULL};
        char *envp[] = {NULL};

        /* Executes same as popen() syscall */
        if (execl(_PATH_BSHELL, "sh", "-c", p->cmd, (char *)NULL) == -1) {
            perror("execl");
        }
        
        exit(0);
    }
}

void process_kill(Process *p) {
    kill_pid(p->pid_node);
}

void kill_pid(pidNode *curr) {
    if (curr) {
        kill(curr->pid, SIGTERM);

        int status;
        waitpid(curr->pid, &status, 0);
        if (curr->prev != NULL) {
            curr->prev->next = curr->next;
        } else {
            head = curr->next;
        }
        if (curr->next != NULL) {
            curr->next->prev = curr->prev;
        }
        free(curr);
    }
}

/**
 * Kill all child processes
 * 
 * Function is triggered by atexit() so that no oprhan processes exists
*/
void kill_processes() {
    while(head) {
        kill_pid(head);
    }
}