#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LineParser.h"
#include "linux/limits.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct variable{
    char* name;
    char* val;
    struct variable *next;	                  /* next process in chain */
} variable;

int dBug_flag = 0;

void dBug_func(int pid, cmdLine* cmd){
    if (dBug_flag){
        printf("PID: %d, Executing command: %s\n", pid, cmd->arguments[0]);
    }
}

void setVar(variable** var_list, cmdLine* cmd){

    variable* var = *var_list;
    while (var != NULL && (strcmp(var->name, cmd->arguments[1]) != 0)){
        var = var->next;
    }

    if (var != NULL){
        free(var->val);
        var->val = strdup(cmd->arguments[2]);
    }
    else
    {
       variable* tmp = malloc(sizeof(variable));
       tmp->name = strdup(cmd->arguments[1]);
       tmp->val =  strdup(cmd->arguments[2]);
       tmp->next = *var_list;
       *var_list = tmp;
    }
    
}

void printVars(variable* var){

    if (var != NULL){
        printVars(var->next);
        printf("Variable name: %s, val: %s\n", var->name, var->val);
    }
}

int replaceVars(cmdLine* cmd, variable* vars){

    int ret = 0;

    for (int i = 0; i < cmd->argCount; i++){
        if (cmd->arguments[i][0] == '$' || cmd->arguments[i][0] == '~'){
            variable* var = vars;
            while (var != NULL && ((strcmp(var->name, (cmd->arguments[i]+1)) != 0) && (strcmp(var->name, cmd->arguments[i]) != 0))){
                var = var->next;
            }

            if (var != NULL){
                replaceCmdArg(cmd, i, var->val);
            }
            else{
                ret = -1;
                printf("Error: no such variable: %s\n", cmd->arguments[i]+1);
            }
        }
    }

    return ret;
}

void freeVars(variable* var){

    if (var != NULL){
        freeVars(var->next);
        free(var->name);
        free(var->val);
        free(var);
    }
    

}

void deleteVar(variable** var_list, char* toDelete){

    variable* var = *var_list;

    if (strcmp(var->name, toDelete) == 0){
        *var_list = var->next;
        var->next = NULL;
        freeVars(var);
    }
    else {

        while (var->next != NULL && (strcmp(var->next->name, toDelete) != 0)){
            var = var->next;
        }

        if (var->next != NULL){
            variable* tmp = var->next;
            var->next = tmp->next;
            tmp->next = NULL;
            freeVars(tmp);
        }
        else {
            printf("Error: no such variable: %s\n", toDelete);
        }
    }
   
}

int execute(cmdLine *pCmdLine) {
    
    pid_t pid = 0;
    int pipefd1[2];
    int pipefd2[2];

    if (pCmdLine->next){
        pipe(pipefd1);
    }

    pipefd2[0] = pipefd1[0];
    pipefd2[1] = pipefd1[1];

    pid = fork();

    if (pid == -1){
        perror("fork failed");
        exit(-1);
    }

    if (pid == 0){

        if (pCmdLine->next){
            dup2(pipefd1[1], fileno(stdout));
        }

        if (pCmdLine->inputRedirect){
            fclose(stdin);
            fopen(pCmdLine->inputRedirect,"r");
        }

        if (pCmdLine->outputRedirect){
            fclose(stdout);
            fopen(pCmdLine->outputRedirect,"w");
        }

        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("error in execution");
            _exit(-1);
        }
    }
    dBug_func(pid, pCmdLine);

    if (pCmdLine->next){
        close(pipefd1[1]);
    }

    while (pCmdLine->next){
        
        pCmdLine = pCmdLine->next;

        pipefd1[0] = pipefd2[0];
        pipefd1[1] = pipefd2[1];

        if (pCmdLine->next){
            pipe(pipefd2);
        }
        pid = fork();

        if (pid == -1){
            perror("fork failed");
            exit(-1);
        }

        if (pid == 0){
            
            dup2(pipefd1[0], fileno(stdin));
            if (pCmdLine->next){
                dup2(pipefd2[1], fileno(stdout));
            }
            
            if (pCmdLine->inputRedirect){
                fclose(stdin);
                fopen(pCmdLine->inputRedirect,"r");
            }

            if (pCmdLine->outputRedirect){
                fclose(stdout);
                fopen(pCmdLine->outputRedirect,"w");
            }

            if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
                perror("error in execution");
                _exit(-1);
            }
        }
        dBug_func(pid, pCmdLine);
        close(pipefd1[0]);
        if (pCmdLine->next){
            close(pipefd2[1]);
        }
    }
    if (pCmdLine->blocking){
        waitpid(pid,NULL,0);
    }
       
    return pid;
}

int command(cmdLine *pCmdLine, variable** var_list) {

    int ret = 0;

    if (strcmp(pCmdLine->arguments[0],"quit") == 0){
        freeCmdLines(pCmdLine);
        freeVars(*var_list);
        exit(0);
    }

    else if (strcmp(pCmdLine->arguments[0],"cd") == 0){
        ret = 1;
        if (chdir(pCmdLine->arguments[1])){
            perror("failed cd");
        }
        
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"set") == 0){
        
        ret = 1;
        setVar(var_list, pCmdLine);      
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"vars") == 0){
        
        ret = 1;
        printVars(*var_list);      
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"delete") == 0){
        
        ret = 1;
        deleteVar(var_list, pCmdLine->arguments[1]);
        freeCmdLines(pCmdLine);
    }

    return ret;
}


int main (int argc , char* argv[], char* envp[]) {

    char* path_buf = malloc(PATH_MAX);
    char* input_buf= malloc(2048);
    int varCheck = 0;
    cmdLine* pCmdLine;
    variable* var_list = malloc(sizeof(variable));

    var_list->name = strdup("~");
    var_list->val = strdup(getenv("HOME"));
    var_list->next = NULL;

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

        cmdLine* tmpCmd = pCmdLine;
        while (tmpCmd != NULL){
            if ((varCheck = replaceVars(tmpCmd, var_list))){
                break;
            }
            tmpCmd = tmpCmd->next;
            
        }

        if (varCheck){
            continue;
        }
        
        if (command(pCmdLine, &var_list)){
            continue;
        }
        
        execute(pCmdLine);
        freeCmdLines(pCmdLine);
    }

}
