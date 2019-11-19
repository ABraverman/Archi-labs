#include "stdio.h"
#include "string.h"
#include "stdlib.h"


int main (int argc, char** argv) {

    char c = 0;
    char dBug = 0;
    char enc = 0;
    char* key;
    char* inFileName;
    char* outFileName;
    size_t size;
    int counter = 0;
    FILE* in = stdin;
    FILE* out = stdout;
    
    for (int i = 1; i < argc; i++){
        if (strncmp(argv[i], "-D", 2) == 0){
            puts(argv[i]);
            dBug = 1;
        }
        if (strncmp(argv[i], "-e", 2) == 0) {
            enc = -1;
            size = strlen(argv[i]) - 2;
            key = malloc(size*sizeof(char));
            strncpy(key, &argv[i][2], size);
            
        }
        if (strncmp(argv[i], "+e", 2) == 0) {
            enc = 1;
            size = strlen(argv[i]) - 2;
            key = malloc(size*sizeof(char));
            strncpy(key, &argv[i][2], size);
        }
        if (strncmp(argv[i], "-i", 2) == 0){
            inFileName = malloc((strlen(argv[i]) - 2)*sizeof(char));
            strncpy(inFileName, &argv[i][2], strlen(argv[i]) - 2);
            in = fopen(inFileName, "r");
        }
        if (strncmp(argv[i], "-o", 2) == 0){
            outFileName = malloc((strlen(argv[i]) - 2)*sizeof(char));
            strncpy(outFileName, &argv[i][2], strlen(argv[i]) - 2);
            out = fopen(outFileName, "w");
        }
    }
    
    while (c != EOF) {
        c = fgetc(in);
        if (dBug && c != EOF)
            fprintf(stderr, "%#04x\t", c);
        if (enc == 0 && c >= 'A' && c <= 'Z')
            c = c + 32;
        else if (enc !=0 && c != EOF) {
                if ( c == '\n'){
                    c = (c + enc*(key[(counter++)%size]))%128;
                    fputc(c,out);
                    counter = 0;
                    c = '\n';
                }
                else
                    c = (c + enc*(key[(counter++)%size]))%128;
            
                if (c < 0)
                    c = c * (-1);
        }
        if (dBug && c != EOF)
            fprintf(stderr, "%#04x\n", c);
        fputc(c,out);
    }
    printf ("\n");
    
    
    return 0;
}
