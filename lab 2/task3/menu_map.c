#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct fun_desc {
  char *name;
  char (*fun)(char);
};

char encrypt(char c); /* Gets a char c and returns its encrypted form by adding 3 to its value. 
          If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c); /* Gets a char c and returns its decrypted form by reducing 3 to its value. 
            If c is not between 0x20 and 0x7E it is returned unchanged */
char xprt(char c); /* xprt prints the value of c in a hexadecimal representation followed by a 
           new line, and returns c unchanged. */
char cprt(char c); /* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed 
                    by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns 
                    the value of c unchanged. */
char my_get(char c); /* Ignores c, reads and returns a character from stdin using fgetc. */ 
char quit(char c);
char censor(char c);
char* map(char *array, int array_length, char (*f) (char));
 
int main(int argc, char **argv){
    
    struct fun_desc menu[] = { { "Censor", censor }, { "Encrypt", encrypt }, { "Decrypt", decrypt },
        { "Print hex", xprt }, { "Print string", cprt }, { "Get string", my_get }, { "Quit", quit }, { "Junk", menu }, { NULL, NULL } };
    char* carray = malloc(5);
    char* tmparray;
    char tmpStr[100];
    int opNum = -1;
    int bound = 0;

    while ((menu+bound)->name != NULL){
        bound++;
    }
    bound -= 1;

    while (1){
        printf("Please choose a function:\n");
        for (int i = 0; (menu + i)->name != NULL; i++) {
            printf("%d) %s\n", i, (menu + i)->name);
        }
        printf("option: ");
    
        fgets(tmpStr, 100, stdin);
        opNum = -1;
        sscanf(tmpStr, "%d", &opNum);

        if ( opNum < 0 || opNum > bound){
            printf("Not within bounds\n");
            quit(1);
        }
        
        sscanf(tmpStr, "%d", &opNum);
        printf("%d\n", opNum);
        printf("Within bounds\n");
                
        tmparray = map(carray, 5, (menu + opNum)->fun);
        free(carray);
        carray = tmparray;
        printf("DONE.\n\n");
    }
}

char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

char encrypt(char c){
    if ( c >= 0x20 && c <= 0x7E) {
        return (c + 3);
    }
    else {
        return c;
    }
}

char decrypt(char c) {
    if ( c >= 0x20 && c <= 0x7E) {
        return (c - 3);
    }
    else {
        return c;
    }
}

char xprt(char c) {
    printf ("%#x\n", c);
    return c;
}

char cprt(char c) {
    if ( c >= 0x20 && c <= 0x7E) {
        printf("%c\n",c);
    }
    else {
        printf(".\n");
    }
    return c;
}

char my_get(char c) {
    return fgetc(stdin);
}

char quit(char c){
    exit(0);
}
 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  for (int i = 0; i < array_length; i++){
      *(mapped_array + i) = f(*(array + i));
  }
  return mapped_array ;
}