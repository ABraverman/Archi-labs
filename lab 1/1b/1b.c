#include "stdio.h"
#include "string.h"

int main (int argc, char * argv[]) {

    int c = 0;
    char dBug = 0;

    for (int i = 1; i < argc; i++){
        
        if (strncmp(argv[i], "-D", 2) == 0){
            dBug = 1;
            puts("-D");
        }
    }
    while ( (c = fgetc(stdin)) != EOF) {
        
        if (dBug){
            fprintf(stderr,"%#02x\t", c);
        }

        if (c <= 'Z' && c>= 'A') {
            c -= ('A'-'a');
        }

        if (dBug){
            fprintf(stderr,"%#02x\n", c);
        }

        fputc(c, stdout);
    }

}