#include <stdio.h>
#include <stdlib.h>

void printHex(char* buffer,int len){
    for (int i = 0; i < len; i++){
        printf("%02hhX ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    
    FILE* file = fopen(argv[1], "r");
    char bBuffer[100];
    int n = 0;
    while(fread(&bBuffer[n], 1, 1, file) != 0){
        n++;
    }
    printHex(bBuffer, n);
    return 0;
}