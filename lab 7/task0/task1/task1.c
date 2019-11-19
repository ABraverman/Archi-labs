#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;


struct fun_desc {
  char *name;
  void (*fun)(state*);
};

void dbug_mode(state* s);
void set_flie(state* s);
void set_units(state* s);
void mem_load(state* s);
void mem_display(state* s);
void file_save(state* s);
void file_mod(state* s);
void quit(state* s);

char* unit_to_format(int unit);
void print_units(FILE* output, char* buffer, int count, int unit_size);

 
int main(int argc, char **argv){
    
    struct fun_desc menu[] = { { "Toggle Debug Mode", dbug_mode }, { "Set File Name", set_flie }, { "Set Unit Size", set_units },
                                { "Load Into Memory", mem_load }, { "Memory Display", mem_display }, { "Save Into File", file_save }, { "File Modify", file_mod }, { "Quit", quit }, { NULL, NULL } };
    char tmpStr[100];
    int opNum = -1;
    int bound = 0;
    state* s = calloc(1, sizeof(state));
    s->unit_size = 1;

    while ((menu+bound)->name != NULL){
        bound++;
    }
    bound -= 1;

    while (1){

        if (s->debug_mode){
            fprintf(stderr, "Dbug: unit size: %d, file name: %s, mem count: %u\n\n", s->unit_size, s->file_name, s->mem_count);
        }

        printf("Please choose a function:\n");
        for (int i = 0; (menu + i)->name != NULL; i++) {
            printf("%d) %s \n", i, (menu + i)->name);
        }
        printf("option: ");
    
        fgets(tmpStr, 100, stdin);
        if (strcmp(tmpStr, "\n") == 0){
            continue;
        }
        opNum = -1;
        sscanf(tmpStr, "%d", &opNum);

        if ( opNum < 0 || opNum > bound){
            printf("Not within bounds\n\n");
            continue;
        }
        
        printf("%d\n", opNum);
        printf("Within bounds\n");
                
        (menu + opNum)->fun(s);
        printf("DONE.\n\n");
    }
}

void dbug_mode(state* s){
  s->debug_mode = 1 - s->debug_mode;
  printf("Debug flag now %s \n", (s->debug_mode ? "on" : "off"));
}

void set_flie(state* s){

    printf("Please enter a file name: ");
    fgets(s->file_name, 100, stdin);
    strtok(s->file_name, "\n");

    if (s->debug_mode){
        fprintf(stderr, "Debug: file name set to: %s\n\n", s->file_name);
    }
}

void set_units(state* s){
    
    char tmp[3];

    printf("Please enter a unit size (1, 2 or 4): ");
    fgets(tmp, 3, stdin);
    if (strncmp(tmp, "1", 1) == 0 || strncmp(tmp, "2", 1) == 0 || strncmp(tmp, "4", 1) == 0){
        sscanf(tmp, "%d", &(s->unit_size));
        if (s->debug_mode){
            fprintf(stderr, "Debug: set size to: %d\n\n", s->unit_size);
        }
    }
    else {
        printf("Illegal unit size argument: %s", tmp);
    }
       
}

void mem_load(state* s){
    
    FILE* file;
    char tmp[100000];
    int loc = -1;
    int len = -1;

    if (!(s->file_name)){
        printf("Please enter a file name\n");
    }
    else if (!(file = fopen(s->file_name, "r"))){
        printf("Failed to open file\n");
    }
    else {
        printf("Please enter <location> <length>:\n");
        fgets(tmp, 100000, stdin);
        sscanf(tmp, "%x %d", &loc, &len);
        if (s->debug_mode){
            fprintf(stderr, "Debug: file name: %s, location: %#x, length: %d\n\n", s->file_name, loc, len);
        }
        fseek(file, loc, SEEK_SET);
        fread(s->mem_buf, s->unit_size, len,file);
        fclose(file);
        printf("Loaded %d units into memory\n", len);
    }
}

void mem_display(state* s){

    char tmp[100000];
    int u = -1;
    int addr = -1;

    printf("Please enter <address> <number of units>:\n");
    fgets(tmp, 100000, stdin);
    sscanf(tmp, "%x %d", &addr, &u);
    if (s->debug_mode){
        fprintf(stderr, "Debug: address: %#x, units: %d\n\n", addr, u);
    }
    
    print_units(stdout,(addr ? (char*)addr : (char*)(s->mem_buf)), u, s->unit_size );
    
    
}

void file_save(state* s){

    FILE* file;
    char tmp[100000];
    int tar_loc = -1;
    int src_addr = -1;
    int len = -1;
    int size;

    if (!(s->file_name)){
        printf("Please enter a file name\n");
    }
    else if (!(file = fopen(s->file_name, "r+"))){
        printf("Failed to open file\n");
    }
    else {
        printf("Please enter <source-address> <target-location> <length>:\n");
        fgets(tmp, 100000, stdin);
        sscanf(tmp, "%x %x %d", &src_addr, &tar_loc, &len);
        if (s->debug_mode){
            fprintf(stderr, "Debug: source-address: %#x, target-location: %#x, length: %d\n\n", src_addr, tar_loc, len);
        }
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        if (size < tar_loc){
            fclose(file);
            printf("Taget location out of file bounds\n");
        }
        else {
            fseek(file, tar_loc, SEEK_SET);
            fwrite((src_addr ? (void*)src_addr : (void*)(s->mem_buf)), s->unit_size, len,file);
            fclose(file);
            printf("Writen %d units into %s\n", len, s->file_name);
        }
        
    }
    
    
}

void file_mod(state* s){

    FILE* file;
    char tmp[100000];
    int location = -1;
    char* value_string;
    long value_int;
    int size;

    if (!(s->file_name)){
        printf("Please enter a file name\n");
    }
    else if (!(file = fopen(s->file_name, "r+"))){
        printf("Failed to open file\n");
    }
    else {
        printf("Please enter <location> <value>:\n");
        fgets(tmp, 100000, stdin);
        sscanf(tmp, "%x %ms", &location, &value_string);
        sscanf(value_string, "%lx", &value_int);
        if (s->debug_mode){
            fprintf(stderr, "Debug: location: %#x, value: %s\n\n", location, value_string);
        }
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        if (size < location){
            fclose(file);
            printf("Taget location out of file bounds\n");
        }
        else {
            fseek(file, location, SEEK_SET);
            size = 0;
            for (int i = 0; *(value_string + i) != 0; i++){
                size++;
            }
            // printf("%d\n", size);
            size = ((size+1)/2)/s->unit_size;
            // size = size/2;
            // printf("%d\n", size);
            fwrite((void*)&value_int, s->unit_size, size, file);
            fclose(file);
            printf("Writen %d units into %s\n", size, s->file_name);
        }
        
    }
    
    
}


void quit(state* s){
    if (s->debug_mode){
        fprintf(stderr, "Quitting\n\n");
    }
    exit(0);
}

char* unit_to_format(int unit) {
    static char* formats[] = {"%hhd\t\t%#04hhx\n", "%hd\t\t%#04hx\n", "No such unit", "%d\t\t%#04x\n"};
    return formats[unit-1];
}

void print_units(FILE* output, char* buffer, int count, int unit_size) {
    char* end = buffer + unit_size*count;
    printf("Decimal\t\tHexdecimal\n");
    printf("==============================\n");
    while (buffer < end) {
        //print ints
        int var = *((int*)(buffer));
        fprintf(output, unit_to_format(unit_size), var, var);
        buffer += unit_size;
    }
}