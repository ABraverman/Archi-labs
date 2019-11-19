#include "stdio.h"
#include "string.h"

int main (int argc, char * argv[]) {

    int c = 0;
    int keyCount = 0;
    char enc = 0;
    char* key;
    char dBug = 0;
    FILE* in = stdin;
    FILE* out = stdout;

    for (int i = 1; i < argc; i++){
        
        if (strncmp(argv[i], "-D", 2) == 0){
            dBug = 1;
            puts("-D");
        }

        if (strncmp(argv[i], "-e", 2) == 0){
            enc = -1;
            key = argv[i]+2;
        }

        if (strncmp(argv[i], "+e", 2) == 0){
            enc = 1;
            key = argv[i]+2;
        }

        if (strncmp(argv[i], "-i", 2) == 0){
            in = fopen(argv[i]+2, "r");
            if ( in == NULL){
                puts ("no such input file");
                return -1;
            }
        }

    }
    while ( (c = fgetc(in)) != EOF) {
        
        if (dBug){
            fprintf(stderr,"%0#x\t", c);
        }

        if (enc == 0 && c <= 'Z' && c>= 'A') {
            c -= ('A'-'a');
        }
        else if (enc != 0) {
            if ( c == '\n'){
                keyCount = 0;
            }
            else {
                c += (enc)*(key[keyCount++]);
                if (key[keyCount] == '\0') {
                    keyCount = 0;
                }
            }
    }

        if (dBug){
            fprintf(stderr,"%0#x\n", c);
        }

        fputc(c, out);
    }

    fclose (in);
    return 0;
}