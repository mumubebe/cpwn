#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pstr.h"

#ifndef PROCESS_H
#define PROCESS_H

#define READ_END 0
#define WRITE_END 1


typedef struct pidNode {
    pid_t pid;
    struct pidNode* next;
    struct pidNode* prev;
} pidNode;

typedef struct {
    pidNode* pid_node;
    pstr* buffer;
    char* cmd;
    int fd_in[2];
    int fd_out[2];
} Process;


pstr* process_recv_raw(Process *p, size_t n, float timeout);
pstr* process_recv(Process *p, size_t n, float timeout);
void process_start(Process *p);
int init_process(Process *p);
int process_can_recv_raw(Process *p, float timeout);
int process_send_raw(Process *p, pstr* ps);
int process_sendline(Process *p, pstr* ps);
void kill_processes();
void kill_pid(pidNode* curr);
void process_kill(Process* p);

#endif