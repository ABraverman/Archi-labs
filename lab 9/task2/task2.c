#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char sig[];
} virus;

struct fun_desc {
  char *name;
  void (*fun)(void);
};

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};

link* globalList = NULL;
     
void printHex(char* buffer,int len){
    for (int i = 0; i < len; i++){
        printf("%02hhX ", buffer[i]);
    }
    printf("\n");
}

     /* Print the data of every link in list. Each item followed by a newline character. */
void list_print(link *virus_list) {

    if (virus_list != NULL){
        list_print(virus_list->nextVirus);
        printf("Virus name: %s\nVirus size: %d\nsignature:\n", virus_list->vir->virusName, virus_list->vir->SigSize);
        printHex(virus_list->vir->sig, virus_list->vir->SigSize);
        printf("\n");
    }
}

     /* Add a new link with the given data to the list 
        (either at the end or the beginning, depending on what your TA tells you),
        and return a pointer to the list (i.e., the first link in the list).
        If the list is null - create a new entry and return a pointer to the entry. */ 
link* list_append(link* virus_list, virus* data){

    link* l = malloc(sizeof(link));
    l->nextVirus = NULL;
    l->vir = data;
    l->nextVirus = virus_list;
    return l;

}

    /* Free the memory allocated by the list. */
void list_free(link *virus_list){
    
    if(virus_list != NULL){
        list_free(virus_list->nextVirus);
        free(virus_list->vir);
        free(virus_list);
    }
}

virus* extractVirus(FILE* file){

    virus* vir = NULL;
    unsigned short size = 0;
    if(fread(&size, 2, 1, file) != 0) {
        vir = malloc(size);
        vir->SigSize = size - 18;
        fread(vir->virusName, size-2, 1, file); 
    }
    return vir;
}

void loadSigs(){

    char fileName[100];
    FILE* file = NULL;
    virus* vir = NULL;

    printf("Please enter a signatures file's name: ");
    fgets(fileName, 100, stdin);
    sscanf(fileName, "%s", fileName);
    file = fopen(fileName,"r");
    if (file == NULL) {
        perror("error reading file");
        exit(-1);
    }
    while ((vir = extractVirus(file)) != NULL) {
        globalList = list_append(globalList, vir);
    }
    fclose(file);

}

void printSigs(){

    list_print(globalList);
}

void detect_virus(char *buffer, unsigned int size){

    link* curr = globalList;
    
    
    while(curr != NULL){
        for (int i = 0; i < (size - curr->vir->SigSize) ; i++){
            if (memcmp(&buffer[i], curr->vir->sig, curr->vir->SigSize) == 0){
                printf("Starting byte: %d\nVirus name: %s\nVirus size: %d\n\n", i, curr->vir->virusName, curr->vir->SigSize);
            }
        }
        curr = curr->nextVirus;        
    }
    
}

void detectVirusShell(){

    char fileName[100];
    char buffer[10000];
    int size;
    FILE* file = NULL;

    printf("Please enter a suspected file's name: ");
    fgets(fileName, 100, stdin);
    sscanf(fileName, "%s", fileName);
    file = fopen(fileName,"r");
    if (file == NULL) {
        perror("error reading file");
        exit(-1);
    }
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (size > sizeof(buffer)) {
        perror("file too big\n");
        exit(-1);
    }
    fread(buffer, size, 1, file);
    detect_virus(buffer, size);


    fclose(file);
}

void kill_virus(char *fileName, int signitureOffset, int signitureSize){

    char nop = 0xc3;
    FILE* file = NULL;
    
    file = fopen(fileName,"r+");
    if (file == NULL) {
        perror("error reading file");
        exit(-1);
    }

    fseek(file, signitureOffset, SEEK_SET);

    // for(int i = 0; i < signitureSize; i++)
    // {
        fwrite(&nop, 1, 1, file);
    // }

    fclose(file);
   
}

void fixFile(){

    char fileName[100];
    char buffer[100];
    int offset = 0;
    int size = 0;
    
    printf("Please enter a suspected file's name: ");
    fgets(fileName, 100, stdin);
    sscanf(fileName, "%s", fileName);
    printf("Please enter the virus's starting byte: ");
    fgets(buffer, 100, stdin);
    sscanf(buffer, "%d", &offset);
    printf("Please enter the virus's signiture size: ");
    fgets(buffer, 100, stdin);
    sscanf(buffer, "%d", &size);
    kill_virus(fileName, offset, size);

}

void quit(){
    list_free(globalList);
    exit(0);
}

int main(int argc, char **argv) {

    struct fun_desc menu[] = { { "Load signatures", loadSigs }, { "Print signatures", printSigs }, { "Detect viruses", detectVirusShell }, { "Fix file", fixFile },
        { "Quit", quit }, { NULL, NULL } };
    char tmpStr[100];
    int opNum = -1;
    int bound = 0;
          
    while ((menu+bound)->name != NULL){
        bound++;
    }
    
    while (1){
        printf("Please choose a function:\n");
        for (int i = 0; (menu + i)->name != NULL; i++) {
            printf("%d) %s\n", i+1, (menu + i)->name);
        }
        printf("option: ");
    
        fgets(tmpStr, 100, stdin);
        opNum = -1;
        sscanf(tmpStr, "%d", &opNum);

        if ( opNum < 1 || opNum > bound){
            perror("Not within bounds\n");
            exit(-1);
        }
        opNum -= 1;
        
        printf("Within bounds\n");
                
        (menu + opNum)->fun();
        printf("DONE.\n\n");
    }
    
    
}