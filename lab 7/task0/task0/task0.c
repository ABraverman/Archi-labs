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
void quit(state* s);

char* unit_to_format(int unit);
void print_units(FILE* output, char* buffer, int count, int unit_size);

 
int main(int argc, char **argv){
    
    struct fun_desc menu[] = { { "Toggle Debug Mode", dbug_mode }, { "Set File Name", set_flie }, { "Set Unit Size", set_units },
                                { "Load Into Memory", mem_load }, { "Memory Display", mem_display }, { "Save Into File", file_save }, { "Quit", quit }, { NULL, NULL } };
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
            printf("Not within bounds\n");
            quit(s);
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