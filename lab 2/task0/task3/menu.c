#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
struct fun_desc {
  char *name;
  char (*fun)(char);
};

char* map(char *array, int array_length, char (*f) (char));
char censor(char c);
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
char quit(char c); /* Gets a char c, and ends the program returning 0 */



int main(int argc, char **argv){
  
    char* carray = malloc(5*sizeof(char));
    char* tmparray;
    char input[10];
    char* string;
    char boundStr[2];
    int boundNum;
    int op;
    
    struct fun_desc funcs[] = { {"Censor", censor}, {"Encrypt", encrypt}, {"Decrypt", decrypt}, 
        {"Print hex", xprt}, {"Print string", cprt}, {"Get string", my_get}, {"Quit", quit},{"Junk", funcs}, {NULL, NULL} };

    sscanf("", "%s", carray);
    boundNum = (sizeof(funcs)/sizeof(struct fun_desc)) - 2;
    
    sprintf(boundStr, "%d", boundNum);
    printf("%d, %c\n", boundNum, *boundStr);


    while(1) {
        for ( int i = 0; (funcs + i)->name != NULL; i++){
            printf("%d) %s\n", i, (funcs + i)->name);
        }

        printf("Option: ");
        fflush(stdin);
        fgets(input, 10, stdin);
        if (*input > *boundStr || *input < '0'){
            printf("Not within bounds\n");
            quit(1);
        }
        sscanf(input, "%ms", &string);
        op = atoi(input);
        
        free(string);
        if (op > boundNum || op < 0){
            printf("Not within bounds\n");
            quit(1);
        }
        else {
            printf("Within bounds\n");
            tmparray = map(carray,5,(funcs + op)->fun);
            free(carray);
            carray = tmparray;
            printf("DONE.\n\n");
        }
    }
    
}

char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}
 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  for (int i = 0; i < array_length; i++) {
      *(mapped_array + i) = f(*(array+i));
  }
  return mapped_array;
}

char encrypt(char c) {
    if (c >= 0x20 && c <= 0x7e){
        c += 3;
    }
    return c;
 
}

char decrypt(char c){
    if (c >= 0x20 && c <= 0x7e){
        c -= 3;
    }
    return c;
}
char xprt(char c){
    
    printf ("%#04X\n", c);
    return c;
}
char cprt(char c) {
    if (c >= 0x20 && c <= 0x7e){
        printf ("%c\n", c);
    }
    else {
        printf (".\n");
    }
    return c;
}
 
char my_get(char c){
    return fgetc(stdin);
}

char quit(char c){
    exit(0);
}