#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tube.h"
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
    char* cmd;
    Tube* tube;
    char* envp[];
} Process;


pstr* process_recv(Process *p, size_t n, float timeout);
pstr* process_recvuntil(Process *p, pstr* pattern, int timeout);
void process_start(Process *p);
int init_process(Process *p);
int process_send(Process *p, pstr* ps);
int process_sendline(Process *p, pstr* ps);
void kill_processes();
void kill_pid(pidNode* curr);
void process_kill(Process* p);

#endif