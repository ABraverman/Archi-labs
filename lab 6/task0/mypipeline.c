#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LineParser.h"
#include "linux/limits.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;


int dBug_flag = 0;




int main (int argc , char* argv[], char* envp[]) {

    char* ls[] = {"ls", "-l", NULL};
    char* tail[] = {"tail", "-n", "2", NULL};
    int pid1 = 0;
    int pid2 = 0;
    int pipefd[2];

    for (int i = 0; i < argc; i++){
        if (strcmp(argv[i],"-d") == 0){
            dBug_flag = 1;
        }
    }

    if (pipe(pipefd)){
        perror("");
    }

    if (dBug_flag){
        fprintf(stderr, "parent_process>forking…\n");
    }
    if ((pid1 = fork()) == 0){
        if (dBug_flag){
        fprintf(stderr, "child1>redirecting stdout to the write end of the pipe…\n");
        }
        fclose(stdout);
        dup(pipefd[1]);
        close(pipefd[1]);
        if (dBug_flag){
            fprintf(stderr, "child1>going to execute cmd: %s\n", ls[0]);
        }
        execvp(ls[0], ls);
        _exit(0);

    }
    if (dBug_flag){
        fprintf(stderr, "parent_process>created process with id: %d\n", pid1);
        fprintf(stderr, "parent_process>closing the write end of the pipe…\n");
    }
    close(pipefd[1]);

    if (dBug_flag){
        fprintf(stderr, "parent_process>forking…\n");
    }
    if ((pid2 = fork()) == 0){
        if (dBug_flag){
        fprintf(stderr, "child2>redirecting stdin to the write end of the pipe…\n");
        }
        fclose(stdin);
        dup(pipefd[0]);
        close(pipefd[0]);
        if (dBug_flag){
            fprintf(stderr, "child2>going to execute cmd: %s\n", tail[0]);
        }
        execvp(tail[0], tail);
        _exit(0);
    }
    if (dBug_flag){
        fprintf(stderr, "parent_process>created process with id: %d\n", pid2);
        fprintf(stderr, "parent_process>closing the read end of the pipe...\n");
    }
    close(pipefd[0]);

    if (dBug_flag){
        fprintf(stderr, "parent_process>waiting for child processes to terminate…\n");
    }
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);


}
