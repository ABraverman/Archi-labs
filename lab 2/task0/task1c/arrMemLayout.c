#include "stdio.h"


int main (int argc, char * argv[]) {
    
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3];

    // int iarray[] = {1,2,3};
    // char carray[] = {'a','b','c'};
    // int* iarrayPtr = iarray;
    // char* carrayPtr = carray;
    // int* p;

    printf("iarray\n");
    for (int i = 0; i < 3; i++) {
        printf("%d, ", *(iarrayPtr + i));
    }
    printf("\n");
    printf("carray\n");
    for (int i = 0; i < 3; i++) {
        printf("%c, ", *(carrayPtr + i));
    }
    printf("\n");
    
    // printf ("iarray: %p\n", iarray);
    // printf ("iarrayPtr: %p\n", iarrayPtr);
    // printf ("carray: %p\n", carray);
    // printf ("carrayPtr: %p\n", carrayPtr);
    // printf ("p: %p\n", p);
    // printf ("&p: %p\n", &p);
    
    printf("iarray: %p\n", iarray);
    printf("iarray + 1: %p\n", (iarray + 1));
    printf("carray: %p\n", carray);
    printf("carray + 1: %p\n", (carray + 1));
}
