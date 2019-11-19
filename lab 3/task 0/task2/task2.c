#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char sig[];
} virus;

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};

struct fun_desc {
  char *name;
  void (*fun)(link**);
};

void printHex(char* buffer,int len){

    for (int i = 0; i < len; i++){
        printf("%02hhX ", buffer[i]);
    }
    printf("\n");
}

    /* Print the data of every link in list. Each item followed by a newline character. */
void list_print(link *virus_list){

    if (virus_list != NULL){
        list_print(virus_list->nextVirus);
        printf("Virus name: %s\nVirus size: %hd\nsignature:\n", virus_list->vir->virusName, virus_list->vir->SigSize);
        printHex(virus_list->vir->sig, virus_list->vir->SigSize);
        printf("\n");
    }
 
}
     
    /* Add a new link with the given data to the list 
        (either at the end or the beginning, depending on what your TA tells you),
        and return a pointer to the list (i.e., the first link in the list).
        If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data){
    
    link* newLink = calloc(1, sizeof(link));
    newLink->vir = data;
    newLink->nextVirus = virus_list;
    return newLink;
}
     
    /* Free the memory allocated by the list. */
void list_free(link *virus_list){
    
    if (virus_list != NULL){
        list_free(virus_list->nextVirus);
        free(virus_list->vir);
        free(virus_list);
    }
    
}

virus* extractVirus(FILE* stream){

    unsigned short SigSize;
    virus* res = NULL;

    if (fread(&SigSize, 2, 1, stream) != 0){
        SigSize -= 18;
        res = malloc(sizeof(virus) + sizeof(char[SigSize]));
        res->SigSize = SigSize;
        fread(res->virusName, 1, 16, stream);
        fread(res->sig, 1, res->SigSize, stream);
    }
    return res;
}

void printSigs(link** virus_list){
    list_print(*virus_list);
}

void loadSigs(link** virus_list){

    char fileName[100];
    FILE* file;
    virus* vir;

    fgets(fileName, 100, stdin);
    sscanf(fileName, "%s", fileName);
    file = fopen(fileName, "r");
    if (file != NULL){
        while((vir = extractVirus(file)) != NULL){
            *virus_list = list_append(*virus_list, vir);
        }
    }
    fclose(file);
}



void detect_virus(char *buffer, unsigned int size, link* virus_list){
    while (virus_list != NULL){
        for (int i = 0; i < size; i++){
            if (memcmp(&buffer[i], virus_list->vir->sig, virus_list->vir->SigSize) == 0){
                printf("location: %d\nvirus name: %s\nVirus size: %d\n\n", i, virus_list->vir->virusName, virus_list->vir->SigSize);
            }
        }
        virus_list = virus_list->nextVirus;
    }
    
}

void detectVir(link** virus_list){
    
    char buffer[10000];
    char fileName[100];
    int size;
    FILE* file;

    fgets(fileName, 100, stdin);
    sscanf(fileName, "%s", fileName);
    file = fopen(fileName, "r");
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    fread(buffer, 1, size, file);
    detect_virus(buffer, size, *virus_list);
    fclose(file);
}

void quit(link** c){
    list_free(*c);
    exit(0);
}

void kill_virus(char *fileName, int signitureOffset, int signitureSize){
    
    char NOP[signitureSize];
    FILE* file;

    for (int i = 0; i < signitureSize; i++){
        NOP[i] = 0x90;
    }
    file = fopen(fileName, "r+");
    fseek(file, signitureOffset, SEEK_SET);
    fwrite(NOP, 1, signitureSize, file);
    fclose(file);
}

void fixFile(link** virus_list){
    
    char buffer[1000];
    char* fileName;
    int offset;
    int size;
    

    fgets(buffer, 1000, stdin);
    sscanf(buffer, "%ms", &fileName);
    fgets(buffer, 1000, stdin);
    sscanf(buffer, "%d", &offset);
    fgets(buffer, 1000, stdin);
    sscanf(buffer, "%d", &size);
    printf("%s, %d, %d\n",fileName, offset, size);
    kill_virus(fileName, offset, size);
    free(fileName);

}

int main(int argc, char **argv) {
    
    struct fun_desc menu[] = { { "Load signatures", loadSigs }, { "Print signatures", printSigs }, { "Detect viruses", detectVir }, { "Fix file", fixFile }, { "Quit", quit }, { NULL, NULL } };

    // FILE* file = fopen("signatures", "r");
    // virus* vir;
    char tmpOp[10];
    int opNum = -1;
    int bound = 0;
    link* virus_list = NULL;

    while ((menu+bound)->name != NULL){
        bound++;
    }

    while (1){
        for (int i = 0; (menu + i)->name != NULL; i++) {
            printf("%d) %s\n", i+1, (menu + i)->name);
        }
    
        fgets(tmpOp, 10, stdin);
        sscanf(tmpOp, "%d", &opNum);
        
        if ( opNum < 1 || opNum > bound){
            printf("Not within bounds\n");
            exit(-1);
        }
        opNum -= 1;
        (menu + opNum)->fun(&virus_list);
    }


    return 0;
}