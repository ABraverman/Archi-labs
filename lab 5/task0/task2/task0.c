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



char dBug_flag = 0;

void dBug(int pid, char* cmd){
    if (dBug_flag){
        fprintf(stderr, "PID: %d, Executing command: %s\n", pid, cmd);
    }
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    
    process* proc = malloc(sizeof(process));
    proc->cmd = cmd;
    proc->pid = pid;
    proc->status = RUNNING;
    proc->next= *process_list;
    *process_list = proc;
}


void freeProcessList(process* process_list){
    if(process_list != NULL){
        freeProcessList(process_list->next);
        if (kill(process_list->pid, SIGINT)){
            perror("Failed kill command");
        }
        freeCmdLines(process_list->cmd);
        free(process_list);
    }
}

void updateProcessList(process **process_list){

    int stat = 0;
    int wait_return = 0;
    process* proc = *process_list;
    while(proc != NULL){
        wait_return = waitpid(proc->pid, &stat, WUNTRACED | WCONTINUED | WNOHANG);
        if (wait_return != 0){
            if (wait_return == -1 || WIFEXITED(stat) || WIFSIGNALED(stat)){
                proc->status = TERMINATED;
            }
            else if(WIFSTOPPED(stat)) {
                proc->status = SUSPENDED;
            }
            else if(WIFCONTINUED(stat)) {
                proc->status = RUNNING;
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

int printProcess(process* proc){
    
    int i = 0;

    if (proc != NULL){
        
        printf("%d\t%d\t", (i = 1 + printProcess(proc->next)), proc->pid);
            switch (proc->status)
            {
                case TERMINATED:
                    printf("TERMINATED\t");
                    break;
                case RUNNING:
                    printf("RUNNING\t\t");
                    break;
                case SUSPENDED:
                    printf("SUSPENDED\t");
                    break;
            
                default:
                    break;
            }
            for (int j = 0; j < proc->cmd->argCount; j++){
                printf("%s ", proc->cmd->arguments[j]);
            }
            printf("\n");

            if (proc->next != NULL && proc->next->status == TERMINATED){
                process* temp = proc->next;
                proc->next=proc->next->next;
                freeCmdLines(temp->cmd);
                free(temp);
            }
    }
    return i;
}

void printProcessList(process** process_list){
    
    updateProcessList(process_list);
    printf("index\tPID\tSTATUS\t\tCommand\n");
    printProcess(*process_list);
    if (*process_list != NULL && (*process_list)->status == TERMINATED){
        process* temp = *process_list;
        *process_list = (*process_list)->next;
        freeCmdLines(temp->cmd);
        free(temp);
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

char path_buf[PATH_MAX];
char input_buf[2048];
pid_t pid;
cmdLine* pCmdLine = NULL;
process* process_list = NULL;

for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i],"-d") == 0){
        dBug_flag = 1;
    }
}


while(1){
    getcwd((char*)&path_buf, PATH_MAX);
    printf("%s>", (char*)&path_buf);
    fgets((char*)&input_buf, 2048, stdin);
    pCmdLine = parseCmdLines((char*)&input_buf);
    if (pCmdLine == NULL){
        continue;
    }

    if (strcmp(pCmdLine->arguments[0],"quit") == 0){
        freeProcessList(process_list);
        freeCmdLines(pCmdLine);
        exit(0);
    }

    if (strcmp(pCmdLine->arguments[0],"cd") == 0){
        if (chdir(pCmdLine->arguments[1])){
            perror("cd failed");
        }
        freeCmdLines(pCmdLine);
        continue;
    }

    if (strcmp(pCmdLine->arguments[0],"procs") == 0){
        printProcessList(&process_list);
        freeCmdLines(pCmdLine);
        continue;
    }

    if (strcmp(pCmdLine->arguments[0],"suspend") == 0){
        if (kill(atoi(pCmdLine->arguments[1]), SIGTSTP)){
            perror("Failed suspend command");
        }
        else{
            updateProcessStatus(process_list, atoi(pCmdLine->arguments[1]), SUSPENDED);
        }
        freeCmdLines(pCmdLine);
        continue;
    }

    if (strcmp(pCmdLine->arguments[0],"kill") == 0){
        if (kill(atoi(pCmdLine->arguments[1]), SIGINT)){
            perror("Failed kill command");
        }
        else{
            updateProcessStatus(process_list, atoi(pCmdLine->arguments[1]), TERMINATED);
        }
        freeCmdLines(pCmdLine);
        continue;
    }

    if (strcmp(pCmdLine->arguments[0],"wake") == 0){
        if (kill(atoi(pCmdLine->arguments[1]), SIGCONT)){
            perror("Failed wake command");
        }
        else {
            updateProcessStatus(process_list, atoi(pCmdLine->arguments[1]), RUNNING);
        }
        freeCmdLines(pCmdLine);
        continue;
    }

    pid = fork();
    
    if (pid == -1){
        printf("fork failed\n");
        freeCmdLines(pCmdLine);
        freeProcessList(process_list);
        exit(-1);
    }
    if (pid == 0){
        execute(pCmdLine);
    }
    dBug(pid, pCmdLine->arguments[0]);
    addProcess(&process_list, pCmdLine, pid);
    if (pCmdLine->blocking){
        waitpid(pid, NULL, 0);
    }
    
}

}
