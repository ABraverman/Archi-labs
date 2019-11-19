#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LineParser.h"
#include "linux/limits.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


char dBug_flag = 0;

void dBug(int pid, char* cmd){
    if (dBug_flag){
        fprintf(stderr, "PID: %d, Executing command: %s\n", pid, cmd);
    }
}

void execute(cmdLine *pCmdLine) {

    int err = execvp(pCmdLine->arguments[0], pCmdLine->arguments);
    if (err == -1) {
        perror("error in task0.execute: ");
        _exit(-1);
    }
    
    return;
}


int main (int argc , char* argv[], char* envp[]) {

char* path_buf = malloc(PATH_MAX);
char* input_buf= malloc(2048);
int pid;
int wstatus;
cmdLine* pCmdLine;

for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i],"-d") == 0){
        dBug_flag = 1;
    }
}


while(1){
    getcwd(path_buf, PATH_MAX);
    printf("%s>",path_buf);
    fgets(input_buf, 2048, stdin);
    pCmdLine = parseCmdLines(input_buf);
    if (pCmdLine == NULL){
        continue;
    }
    if (strcmp(pCmdLine->arguments[0],"quit") == 0){
        exit(0);
    }
    if (strcmp(pCmdLine->arguments[0],"cd") == 0){
        if (chdir(pCmdLine->arguments[1])){
            perror("cd failed");
        }
        continue;
    }
    pid = fork();
    if (pid == -1){
        printf("fork failed\n");
        exit(-1);
    }
    if (pid == 0){
        execute(pCmdLine);
    }
    dBug(pid, pCmdLine->arguments[0]);
    if (pCmdLine->blocking){
        waitpid(pid, &wstatus, 0);
    }
    freeCmdLines(pCmdLine);
}

}
