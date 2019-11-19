#include "stdio.h"
#include "string.h"

int main (int argc, char ** argv[]) {

    int c = 0;

    
    while ( (c = fgetc(stdin)) != EOF) {
        
        if (c <= 'Z' && c>= 'A') {
            c -= ('A'-'a');
        }

        fputc(c, stdout);
    }

}