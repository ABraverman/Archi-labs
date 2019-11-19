#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count(char* c);

int main(int argc, char **argv){
    printf("%d\n", count(argv[1]));

}

int count(char* c){
    int count = 0;
    for (int i = 0; *(c+i) != 0; i++){
        if (*(c+i) >= '0' && *(c+i) <= '9'){
            count++;
        }
    }

    return count;
}

