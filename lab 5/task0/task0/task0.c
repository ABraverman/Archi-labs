#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LineParser.h"
#include "linux/limits.h"
#include <unistd.h>


void execute(cmdLine *pCmdLine) {

    int err = execvp(pCmdLine->arguments[0], pCmdLine->arguments);
    if (err == -1) {
        perror("error in task0.execute");
        exit(-1);
    }
    
    return;
}


int main (int argc , char* argv[], char* envp[]) {

char* path_buf = malloc(PATH_MAX);
char* input_buf= malloc(2048);
cmdLine* pCmdLine;

while(1){
    getcwd(path_buf, PATH_MAX);
    printf("%s>",path_buf);
    fgets(input_buf, 2048, stdin);
    pCmdLine = parseCmdLines(input_buf);
    if (strcmp(pCmdLine->arguments[0],"quit") == 0){
        exit(0);
    }
    execute(pCmdLine);
    freeCmdLines(pCmdLine);
}

}
