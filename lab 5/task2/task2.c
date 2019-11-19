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

int command(cmdLine *pCmdLine, process** process_list) {

    int ret = 0;

    if (strcmp(pCmdLine->arguments[0],"quit") == 0){
        freeCmdLines(pCmdLine);
        freeProcessList(*process_list);
        exit(0);
    }

    else if (strcmp(pCmdLine->arguments[0],"cd") == 0){
        ret = 1;
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

    return ret;
}


int main (int argc , char* argv[], char* envp[]) {

char* path_buf = malloc(PATH_MAX);
char* input_buf= malloc(2048);
int pid = 0;
cmdLine* pCmdLine;
process* process_list = NULL;

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
    
    if (command(pCmdLine, &process_list)){
        continue;
    }
    
    pid = execute(pCmdLine);
    addProcess(&process_list, pCmdLine, pid);
}

}
