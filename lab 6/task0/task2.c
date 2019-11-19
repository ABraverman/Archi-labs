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

typedef struct variable{
    char* name;
    char* val;
    struct variable *next;	                  /* next var in chain */
} variable;


int dBug_flag = 0;

void dBug_func(int pid, cmdLine* cmd){
    if (dBug_flag){
        printf("PID: %d, Executing command: %s\n", pid, cmd->arguments[0]);
    }
}

void freeProcessList(process* process_list){

    if (process_list != NULL){
        freeProcessList(process_list->next);
        kill(process_list->pid, SIGINT);
        freeCmdLines(process_list->cmd);
        free(process_list);
        
    }
}

void updateProcessList(process **process_list){

    process* proc = *process_list;
    int status = 0;
    int wait_ret = 0;

    while(proc != NULL){
        
        if ((wait_ret = waitpid(proc->pid, &status, WNOHANG | WUNTRACED | WCONTINUED))){
            if (wait_ret == -1){
                proc->status = TERMINATED;
            }
            if (WIFSTOPPED(status)){
                proc->status = SUSPENDED;
            }
            else if (WIFCONTINUED(status)){
                proc->status = RUNNING;
            }
            else {
                proc->status = TERMINATED;
            }
        }
        proc = proc->next;
    }
}

void updateProcessStatus(process* process_list, int pid, int status){

    while (process_list != NULL && process_list->pid != pid){
        process_list = process_list->next;
    }
    if (process_list == NULL){
        return;
    }

    process_list->status = status;
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){

    process* proc = malloc(sizeof(process));
    proc->cmd = cmd;
    proc->pid = pid;
    proc->status = RUNNING;
    proc->next = *process_list;
    *process_list = proc;
}

void setVariable(variable** variable_list, cmdLine* cmd){

    if (cmd->argCount >= 3){
        variable* var = *variable_list; 
        while(var != NULL && strcmp(var->name, cmd->arguments[1]) != 0){
            var = var->next;
        }

        if (var){
            char* temp = NULL;
            sscanf(cmd->arguments[2], "%ms", &temp);
            free(var->val);
            var->val = temp;
        }
        else {
            char* temp = NULL;
            sscanf(cmd->arguments[1], "%ms", &temp);
            variable* var = malloc(sizeof(variable));
            var->name = temp;
            sscanf(cmd->arguments[2], "%ms", &temp);
            var->val = temp;
            var->next = *variable_list;
            *variable_list = var;
        }
    }
    else {
        printf("Insufficent amount of vars\n");
    }
}

int printProcess(process* process_list){
    
    int index = 0;

    if (process_list != NULL){
        printf("%d\t%d\t", (index = 1 + printProcess(process_list->next)), process_list->pid);
        switch (process_list->status)
        {
            case RUNNING:
                printf("RUNNING\t\t");
                break;
            case TERMINATED:
                printf("TERMINATED\t");
                break;
            case SUSPENDED:
                printf("SUSPENDED\t");
                break;
        
            default:
                break;
        }
        for (int i = 0; i < process_list->cmd->argCount; i++){
            printf("%s ", process_list->cmd->arguments[i]);
        }
        printf("\n");
    }

    return index;
}

void delete_terminated(process* proc){

    process* temp = NULL;

    if (proc != NULL){
        delete_terminated(proc->next);
        if (proc->next != NULL && proc->next->status == TERMINATED){
            temp = proc->next;
            proc->next = proc->next->next;
            temp->next = NULL;
            freeProcessList(temp);
        }
        
    } 
}
void printProcessList(process** process_list){
    
    process* temp = NULL;

    updateProcessList(process_list);
    printf("Index\tPID\tStatus\t\tCommand\n");
    printProcess(*process_list);

    delete_terminated(*process_list);

    if (*process_list != NULL && (*process_list)->status == TERMINATED){
        temp = *process_list;
        *process_list = (*process_list)->next;
        temp->next = NULL;
        freeProcessList(temp);
    }
}


void printVaribaleList(variable** var_list){
    
    variable* temp = *var_list;

    while(temp != NULL){
        printf("Variable name: %s, value: %s\n", temp->name,temp->val);
        temp = temp->next;
    }
}

int replaceVars(cmdLine* cmd, variable* var){
    
    int ret = 0;

    for (int i = 0; i < cmd->argCount; i++){
        if (cmd->arguments[i][0] == '$'){
            while (var != NULL && strcmp(var->name, (cmd->arguments[i]+1)) != 0){
                var = var->next;
            }
            if (var){
                replaceCmdArg(cmd, i, var->val);
            }
            else{
                printf("Var \"%s\" not found\n", (cmd->arguments[i]+1));
                ret = -1;
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

int deleteVar(variable** var_list, char* toDelete){

    variable* var = *var_list;
    int ret = 0;

    if (strcmp(var->name, toDelete) == 0){
        *var_list = var->next;
        var->next = NULL;
        freeVars(var);
    }
    else {
        while (var != NULL && var->next != NULL && strcmp(var->next->name, toDelete) != 0){
            var = var->next;
        }
        if (var->next){
           variable* temp = var->next;
           var->next = temp->next;
           temp->next = NULL;
           freeVars(temp);  
        }
        else{
             printf("Var \"%s\" not found\n", toDelete);
                ret = -1;
        }
    }

    return ret;
}

int execute(cmdLine *pCmdLine) {
    
    pid_t pid1 = 0;
    pid_t pid2 = 0;
    int pipefd[2];

    if (pCmdLine->next){
        pipe(pipefd);
    }

    pid1 = fork();

    if (pid1 == -1){
        perror("fork failed");
        exit(-1);
    }

    if (pid1 == 0){

        if (pCmdLine->next){
            fclose(stdout);
            dup(pipefd[1]);
            close(pipefd[1]);
        }

        if (pCmdLine->inputRedirect){
            fclose(stdin);
            fopen(pCmdLine->inputRedirect, "r");
        }
        if (pCmdLine->outputRedirect){
            fclose(stdout);
            fopen(pCmdLine->outputRedirect, "w");
        }
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("error in execution");
            _exit(-1);
        }
    }
    if (pCmdLine->next){
        close(pipefd[1]);
    }
    dBug_func(pid1, pCmdLine);
    

    if (pCmdLine->next){
        
        pCmdLine = pCmdLine->next;

        pid2 = fork();

        if (pid2 == -1){
            perror("fork failed");
            exit(-1);
        }

        if (pid2 == 0){

            fclose(stdin);
            dup(pipefd[0]);
            close(pipefd[0]);
            
            if (pCmdLine->inputRedirect){
                fclose(stdin);
                fopen(pCmdLine->inputRedirect, "r");
            }
            if (pCmdLine->outputRedirect){
                fclose(stdout);
                fopen(pCmdLine->outputRedirect, "w");
            }
            if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
                perror("error in execution");
                _exit(-1);
            }
        }
        dBug_func(pid2, pCmdLine);
        close(pipefd[0]);
        

    }

    if (pCmdLine->blocking){
        waitpid(pid1,NULL,0);
        if (pCmdLine->next){
            waitpid(pid2,NULL,0);
        }
    }

    
    
    return pid1;
}

int command(cmdLine *pCmdLine, process** process_list, variable** variable_list) {

    int ret = 0;

    if (strcmp(pCmdLine->arguments[0],"quit") == 0){
        freeCmdLines(pCmdLine);
        freeProcessList(*process_list);
        freeVars(*variable_list);
        exit(0);
    }

    else if (strcmp(pCmdLine->arguments[0],"cd") == 0){
        ret = 1;
        if (strcmp(pCmdLine->arguments[1], "~") == 0){
            replaceCmdArg(pCmdLine, 1, getenv("HOME"));
        }
        if (chdir(pCmdLine->arguments[1])){
            perror("failed cd");
        }
        
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"procs") == 0){
        ret = 1;
        printProcessList(process_list);
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"suspend") == 0){
        ret = 1;
        if (kill(atoi(pCmdLine->arguments[1]), SIGTSTP)){
            perror("failed suspend");
        }
        // updateProcessStatus(*process_list, atoi(pCmdLine->arguments[1]), SUSPENDED);
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"kill") == 0){
        ret = 1;
        if (kill(atoi(pCmdLine->arguments[1]), SIGINT)){
            perror("failed kill");
        }
        // updateProcessStatus(*process_list, atoi(pCmdLine->arguments[1]), TERMINATED);
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"wake") == 0){
        ret = 1;
        if (kill(atoi(pCmdLine->arguments[1]), SIGCONT)){
            perror("failed wake");
        }
        // updateProcessStatus(*process_list, atoi(pCmdLine->arguments[1]), RUNNING);
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"set") == 0){
        ret = 1;
        setVariable(variable_list,pCmdLine);
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"vars") == 0){
        ret = 1;
        printVaribaleList(variable_list);
        freeCmdLines(pCmdLine);
    }

    else if (strcmp(pCmdLine->arguments[0],"delete") == 0){
        ret = 1;
        deleteVar(variable_list, pCmdLine->arguments[1]);
        freeCmdLines(pCmdLine);
    }

    return ret;
}




int main (int argc , char* argv[], char* envp[]) {

char path_buf[PATH_MAX];
char input_buf[2048];
int pid = 0;
cmdLine* pCmdLine;
process* process_list = NULL;
variable* var_list = NULL;

for (int i = 0; i < argc; i++){
    if (strcmp(argv[i],"-d") == 0){
        dBug_flag = 1;
    }
}

while(1){
    getcwd((char*)path_buf, PATH_MAX);
    printf("%s>",(char*)path_buf);
    fgets((char*)input_buf, 2048, stdin);
    pCmdLine = parseCmdLines((char*)input_buf);

    if (pCmdLine == NULL){
        continue;
    }

    if (replaceVars(pCmdLine, var_list)){
        continue;
    }
    
    if (command(pCmdLine, &process_list, &var_list)){
        continue;
    }
    
    pid = execute(pCmdLine);
    addProcess(&process_list, pCmdLine, pid);
}

}
