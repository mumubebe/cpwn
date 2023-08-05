#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <poll.h>
#include <paths.h>
#include <signal.h>
#include "str.h"
#include "process.h"
#include "tube.h"
#include <time.h>
#include <sys/select.h> 
#include <termios.h>
#include "log.h"

struct termios tios;

pidNode* head = NULL;
str* buffer = NULL;

#define READ_BLOCK 0
#define READ_NONBLOCK 1

// TODO: Make this as a setting 
#define TUBE_PTY 1

int process_can_recv_raw(Tube *tb, float timeout);
str* process_recv_raw(Tube* tb, size_t n, float timeout);



/*
 * Recv number `n` bytes from process
 * 
 * if timeout or EOF is reached, return value points to an empty string.
*/
str* process_recv_raw(Tube *tb, size_t n, float timeout) {
    if (process_can_recv_raw(tb, timeout)) {
        char* buf = malloc(sizeof(char) * n);
        str* rrecv;

        if (!buf) {
            perror("malloc");
            return NULL;
        }
        size_t len;
        len = read(tb->fd_out, buf, n-1);
        
        if (len == -1) {
            perror("read");
            rrecv = str_new("");
        }

        if (len) {
            rrecv = str_new_raw(buf, len);
        }
        free(buf);
        return rrecv;
    }
    return str_new("");
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
        LOG_DEBUG("process_can_recv_raw(): Data is not available from process %d", tb->pid_node->pid);
        return 0;
    }

    if (fds[0].revents & POLLIN) {
        LOG_DEBUG("process_can_recv_raw(): Data available from process %d", tb->pid_node->pid);
        return 1;
    }
    return 0;
}


/**
 * fork a process as PIPE IO or pty (pseudo-terminal) which circumvent the default block buffering performed by the 
 * stdio functions when writing output to a disk file or pipe,
 * as opposed to the line buffering used for terminal output
*/
Tube* pwn_process(char *cmd) {
    // pty
    int fdm, fds;
    // for pipe
    int fd_out[2];
    int fd_in[2];
    pid_t pid;
    
    if (TUBE_PTY) {
        // Open a new pseudoterminal
        if ((fdm = posix_openpt(O_RDWR | O_NOCTTY)) == -1) {
            perror("posix_openpt");
            exit(EXIT_FAILURE);
        }

        // Grant access to the slave pseudoterminal
        if (grantpt(fdm) == -1) {
            perror("grantpt");
            exit(EXIT_FAILURE);
        }

        // Get the current terminal attributes
        if (tcgetattr(fdm, &tios) < 0) {
            perror("tcgetattr");
            exit(EXIT_FAILURE);
        }

        // https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
        // raw mode
        tios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        tios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        tios.c_cflag &= ~(CSIZE | PARENB);
        tios.c_cflag |= CS8;
        tios.c_oflag &= ~(OPOST);
        tios.c_oflag &= ~ONLCR; // disable \r


        // Set the new attributes
        if (tcsetattr(fdm, TCSANOW, &tios) < 0) {
            perror("tcsetattr");
            exit(EXIT_FAILURE);
        }

        // Unlock the slave pseudoterminal
        if (unlockpt(fdm) == -1) {
            perror("unlockpt");
            exit(EXIT_FAILURE);
        }

        char *slave_name;
        // get slave pty name
        if ((slave_name = ptsname(fdm)) == NULL) {
            perror("ptsname_r");
            exit(EXIT_FAILURE);
        }

        // open slave pty
        if ((fds = open(slave_name, O_RDWR | O_NOCTTY)) == -1) {
            perror("open slave");
            exit(EXIT_FAILURE);
        }

    } else {

        if (pipe(fd_out) == -1 || pipe(fd_in) == -1) {
            perror("pipe");
        }
    }

    // Fork a child process
    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    // Child process
    if (pid == 0) {
        if (TUBE_PTY) {
            close(fdm);  // Close unused file descriptor

            // Make the current process a new session leader
            setsid();

            // Set the controlling terminal to the slave pseudoterminal
            ioctl(fds, TIOCSCTTY, 0);

            close(0); 
            close(1); 
            close(2); 
            
            dup2(fds, STDIN_FILENO);
            dup2(fds, STDOUT_FILENO);
            dup2(fds, STDERR_FILENO);

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
        }

        
        /* Executes same as popen() syscall */
        if (execl(_PATH_BSHELL, "sh", "-c", cmd, (char *)NULL) == -1) {
            perror("execl");
        }

        exit(EXIT_FAILURE);

    // Parent
    } else {  

        // Add forked process to a pidNode linked list to keep track of all forked processes
        Tube* tube = malloc(sizeof(Tube)); 
        tube->type = PROCESS_TUBE;
        
        /* init empty buffer to tube struct */
        tube->buffer = str_new("");       

        /* Stores file descriptors in current struct */
        if (TUBE_PTY) {
            close(fds);
            tube->fd_out = fdm;
            tube->fd_in = fdm;
        } else {
            close(fd_in[READ_END]);
            close(fd_out[WRITE_END]);

            tube->fd_out = fd_out[READ_END];
            tube->fd_in = fd_in[WRITE_END];
        } 

        // Add forked process to a pidNode linked list to keep track of all forked processes
        pidNode* pid_node = malloc(sizeof(pidNode));
        if (!pid_node) { perror("malloc"); return NULL; }

        pid_node->pid = pid;
        pid_node->next = head;
        pid_node->prev = NULL;

        tube->pid_node = pid_node;
        
        // Register to kill all processes when program exits
        if (head == NULL) {
            atexit(kill_processes);
        }

        if (head != NULL) {
            head->prev = pid_node;
        }
        head = pid_node;

        LOG_INFO("Starting local process '%s': pid %d", cmd, pid);

        return tube;
    }

    return NULL;
}

void pwn_interactive(Tube* tb) {
    LOG_INFO("Switching to interactive mode");
    char buffer[1024];
    fd_set set;

    while (1) {
        FD_ZERO(&set);  // Clear the set
        FD_SET(tb->fd_in, &set);  // Add the master pseudoterminal to the set
        FD_SET(STDIN_FILENO, &set);  // Add the standard input to the set

        if (select(tb->fd_in + 1, &set, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }

        if (FD_ISSET(tb->fd_in, &set)) {  // If data is available from the child process
            ssize_t bytes = read(tb->fd_in, buffer, sizeof(buffer) - 1);
            if (bytes <= 0) {
                LOG_INFO("Got EOF while reading in interactive");
                break;
            };  

            buffer[bytes] = '\0';  
            printf("%s", buffer);  
        }

        if (FD_ISSET(STDIN_FILENO, &set)) {  // If data is available from the user
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {  
                write(tb->fd_in, buffer, strlen(buffer));  // Write to the master pseudoterminal
            }
        }
    }
}

void process_kill(Tube* tb) {
    kill_pid(tb->pid_node);
}

void kill_pid(pidNode *curr) {
    if (curr) {
        kill(curr->pid, SIGTERM);
        
        LOG_INFO("Stopped process (pid %d)", curr->pid);


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