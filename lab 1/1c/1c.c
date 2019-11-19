#include "stdio.h"
#include "string.h"

int main (int argc, char * argv[]) {

    int c = 0;
    int keyCount = 0;
    char enc = 0;
    char keyPos = 0;
    char dBug = 0;

    for (int i = 1; i < argc; i++){
        
        if (strncmp(argv[i], "-D", 2) == 0){
            dBug = 1;
            puts("-D");
        }

        if (strncmp(argv[i], "-e", 2) == 0){
            enc = -1;
            keyPos = i;
        }

        if (strncmp(argv[i], "+e", 2) == 0){
            enc = 1;
            keyPos = i;
        }
    }
    while ( (c = fgetc(stdin)) != EOF) {
        
        if (dBug){
            fprintf(stderr,"%#x\t", c);
        }

        if (enc == 0 && c <= 'Z' && c>= 'A') {
            c -= ('A'-'a');
        }
        else if (enc != 0) {
            if ( c == '\n'){
                c += (enc)*(*(&argv[keyPos][2] + keyCount++));
                fputc(c, stdout);
                keyCount = 0;
                c = '\n';
            }
            else {
                c += (enc)*(*(&argv[keyPos][2] + keyCount++));
                if (*(&argv[keyPos][2] + keyCount) == '\0') {
                    keyCount = 0;
                }
            }
    }

        if (dBug){
            fprintf(stderr,"%#x\n", c);
        }

        fputc(c, stdout);
    }

}