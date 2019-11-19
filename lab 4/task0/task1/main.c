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


extern int system_call();

void deBug (int ID, int code){
  
  char* str;

  system_call(SYS_WRITE, STDERR, "ID: ", 4);
  str = itoa(ID);
  system_call(SYS_WRITE, STDERR, str, strlen(str));
  system_call(SYS_WRITE, STDERR, ", Return code: ", strlen(", Return code: "));
  str = itoa(code);
  system_call(SYS_WRITE, STDERR, str, strlen(str));
  system_call(SYS_WRITE, STDERR, "\n", 1);
}

int main (int argc , char* argv[], char* envp[])
{
  char c = 0;
  char dBugFlag = 0;
  char buffer[4000];
  int in = STDIN;
  int out = STDOUT;
  int code;
  int i;
  int buffIndex = 0;

  for(i = 1; i < argc; i++){
    if (strcmp(argv[i], "-D") == 0){
      dBugFlag = 1;
    }
  }
  for(i = 1; i < argc; i++){
    if (strncmp(argv[i], "-i", 2) == 0){
      in = system_call(SYS_OPEN, (argv[i]+2), O_RDONLY, 0777);
      if (in < 0){
        return 0X55;
      }
    }
    if (strncmp(argv[i], "-o", 2) == 0){
      out = system_call(SYS_OPEN, (argv[i]+2), O_WRONLY | O_CREAT | O_TRUNC, 0777);
      if (out < 0){
        return 0X55;
      }
    }
  }
  
  while (c != 10){
    code = system_call(SYS_READ, in, &c, 1);
    if (dBugFlag){
      deBug(SYS_READ, code);
    }
    if ( c >= 'A' && c<= 'Z'){
      c += ('a' - 'A');
    }
    buffer[buffIndex++] = c;
    if (buffIndex == 4000){
      code = system_call(SYS_WRITE, out, buffer, buffIndex);
      if (dBugFlag){
        deBug(SYS_WRITE, code);
      }
    }
  }
  code = system_call(SYS_WRITE, out, buffer, buffIndex);
  if (dBugFlag){
    deBug(SYS_WRITE, code);
  }

  
  
  return 0;
}
