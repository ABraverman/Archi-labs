#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LineParser.h"
#include "linux/limits.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int dBug_flag = 0;

void dBug_func(int pid, cmdLine* cmd){
    if (dBug_flag){
        printf("PID: %d, Executing command: %s\n", pid, cmd->arguments[0]);
    }
}

int execute(cmdLine *pCmdLine) {
    
    pid_t pid = 0;

    pid = fork();

    if (pid == -1){
        perror("fork failed");
        exit(-1);
    }

    if (pid == 0){
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("error in execution");
            _exit(-1);
        }
    }
    dBug_func(pid, pCmdLine);
    if (pCmdLine->blocking){
        waitpid(pid,NULL,0);
    }
    
    
    return pid;
}

int command(cmdLine *pCmdLine) {

    int ret = 0;

    if (strcmp(pCmdLine->arguments[0],"quit") == 0){
        freeCmdLines(pCmdLine);
        exit(0);
    }

    if (strcmp(pCmdLine->arguments[0],"cd") == 0){
        ret = 1;
        if (chdir(pCmdLine->arguments[1])){
            perror("failed cd");
        }
        
        freeCmdLines(pCmdLine);
    }
    return ret;
}


int main (int argc , char* argv[], char* envp[]) {

char* path_buf = malloc(PATH_MAX);
char* input_buf= malloc(2048);
int pid = 0;
cmdLine* pCmdLine;

for (int i = 0; i < argc; i++){
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
    
    if (command(pCmdLine)){
        continue;
    }
    
    pid = execute(pCmdLine);
    freeCmdLines(pCmdLine);
}

}
