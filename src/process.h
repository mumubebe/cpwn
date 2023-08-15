#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tube.h"
#include "str.h"

#define READ_END 0
#define WRITE_END 1

extern str *process_recv_raw(Tube *tb, size_t n, float timeout);
extern Tube *pwn_process(char *cmd);
extern void pwn_interactive(Tube *tb);
extern void kill_processes();
extern void kill_pid(pidNode *curr);
extern void process_kill(Tube *tb);

#endif