#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tube.h"
#include "pstr.h"


#define READ_END 0
#define WRITE_END 1


pstr*   process_recv_raw(Tube *tb, size_t n, float timeout);
Tube*   pwn_process(char* cmd);
void    pwn_interactive(Tube *tb);
void    kill_processes();
void    kill_pid(pidNode* curr);
void    process_kill(Tube* tb);

#endif