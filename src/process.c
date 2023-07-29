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

pidNode* head = NULL;
pstr* buffer = NULL;

#define READ_BLOCK 0
#define READ_NONBLOCK 1

int process_can_recv_raw(Tube *tb, float timeout);
pstr* process_recv_raw(Tube* tb, size_t n, float timeout);



/*
 * Recv number `n` bytes from process
 * 
 * if timeout or EOF is reached, return value points to an empty string.
*/
pstr* process_recv_raw(Tube *tb, size_t n, float timeout) {
    if (process_can_recv_raw(tb, timeout)) {
        char* buf = malloc(sizeof(char) * n);
        
        if (!buf) {
            perror("malloc");
            return NULL;
        }
        size_t len;
        len = read(tb->fd_out, buf, n-1);
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
int process_can_recv_raw(Tube* tb, float timeout) {
    /* Setup poll */
    struct pollfd fds[1];

    fds[0].fd = tb->fd_out;
    fds[0].events = POLLIN;

    // poll takes timeout as millisecs
    int ret = poll(fds, 1, timeout*1000.0f);

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


Tube* pwn_process(char *cmd) {
    int fd_out[2];
    int fd_in[2];

    if (pipe(fd_out) == -1 || pipe(fd_in) == -1) {
        perror("pipe");
    }


    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed:");
        return NULL;
    } 
    else if (pid > 0) {
        /* Add forked process to a pidNode linked list to keep track of all forked processes */
        Tube* tube = malloc(sizeof(Tube)); 
        tube->type = PROCESS_TUBE;
        
        /* init empty buffer to tube struct */
        tube->buffer = pstr_new("");       
        
        /* Stores file descriptors in current struct */
        tube->fd_out = fd_out[READ_END];
        tube->fd_in = fd_in[WRITE_END];

        /* Add forked process to a pidNode linked list to keep track of all forked processes */
        pidNode* pid_node = malloc(sizeof(pidNode));
        if (!pid_node) { perror("malloc"); return NULL; }

        pid_node->pid = pid;
        pid_node->next = head;
        pid_node->prev = NULL;

        tube->pid_node = pid_node;

        if (head != NULL) {
            head->prev = pid_node;
        }
        head = pid_node;

        close(fd_in[READ_END]);
        close(fd_out[WRITE_END]);

        
        /* Register kill process atexit */
        atexit(kill_processes);

        return tube;
    } 
    else {
        /* Duplicate FD to FD2, closing FD2 and making it open on the same file */
        
        close(fd_in[WRITE_END]);
        close(fd_out[READ_END]);

        dup2(fd_in[READ_END], STDIN_FILENO);
        dup2(fd_out[WRITE_END], STDOUT_FILENO);
        dup2(fd_out[WRITE_END], STDERR_FILENO);

        close(fd_in[READ_END]);
        close(fd_out[WRITE_END]);
        

        /* Executes same as popen() syscall */
        if (execl(_PATH_BSHELL, "sh", "-c", cmd, (char *)NULL) == -1) {
            perror("execl");
        }
        
        exit(0);
    }
}

void process_kill(Tube* tb) {
    kill_pid(tb->pid_node);
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