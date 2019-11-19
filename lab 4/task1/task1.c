#include "util.h"

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_APPEND 1024
#define O_TRUNC 512
#define O_CREAT 64

#define NULL (void*)0


extern int system_call(int,...);

char dBugFlag = 0;
char* inFile = "stdin";
char* outFile = "stdout";

void deBug (int ID, int code){
    
    char* str;

    if (dBugFlag){
        system_call(SYS_WRITE, STDERR, "\n", 1);
        system_call(SYS_WRITE, STDERR, "ID: ", 4);
        str = itoa(ID);
        system_call(SYS_WRITE, STDERR, str, strlen(str));
        system_call(SYS_WRITE, STDERR, ", code: ", 8);
        str = itoa(code);
        system_call(SYS_WRITE, STDERR, str, strlen(str));
        system_call(SYS_WRITE, STDERR, " in file path: ", 15);
        system_call(SYS_WRITE, STDERR, inFile, strlen(inFile));
        system_call(SYS_WRITE, STDERR, " out file path: ", 16);
        system_call(SYS_WRITE, STDERR, outFile, strlen(outFile));
        system_call(SYS_WRITE, STDERR, "\n", 1);
    }
}

int main (int argc , char* argv[], char* envp[]) {



char c = 1;
int code = 1;
int out = STDOUT;
int in = STDIN;
int i;

for (i = 0; i < argc; i++){

    if ((strcmp(argv[i], "-D") == 0)){
        dBugFlag = 1;
    }

}
for (i = 0; i < argc; i++){

    if ((strncmp(argv[i], "-i", 2) == 0)){
        inFile = argv[i] + 2;
        in = system_call(SYS_OPEN, argv[i] + 2, O_RDONLY, 0777);
        deBug(SYS_OPEN, in);
    }

    if ((strncmp(argv[i], "-o", 2) == 0)){
        outFile = argv[i] + 2;
        out = system_call(SYS_OPEN, argv[i] + 2, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        deBug(SYS_OPEN, out);
    }

}

code = system_call(SYS_READ, in, &c, 1);
deBug(SYS_READ, code);

while (code > 0) {

    if (c <= 'Z' && c >= 'A') {
        c += 'a' - 'A';
    }

    code = system_call(SYS_WRITE, out, &c, 1);
    deBug(SYS_WRITE, code);

    code = system_call(SYS_READ, in, &c, 1);
    deBug(SYS_READ, code);
    
}

if (in != STDIN){
    code = system_call(SYS_CLOSE, in);
    deBug(SYS_CLOSE, code);
}

if (out != STDOUT){
    code = system_call(SYS_CLOSE, out);
    deBug(SYS_CLOSE, code);
}

return 0;

}
