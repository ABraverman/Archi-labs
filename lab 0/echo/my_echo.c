#include <stdio.h>

int main(int argc, char *argv[]) {
    
    int i = 1;
    
    while (i < argc)
        fputs(argv[i++]);
    printf("\n");
    return 0;
}
