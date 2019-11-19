#include "util.h"

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define STDOUT 1
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{
  char* fileName = argv[1];
  char* xName = argv[2];
  char clean = 0;
  int fileDesc;
  int fileSize;
  int leftToDelete = strlen("shira.\n") - (strlen(xName) + 2);

  fileDesc = system_call(SYS_OPEN, fileName, 2, 0777);
  if (fileDesc == -1){
    return 0x55;
  }
  fileSize = system_call(SYS_LSEEK, fileDesc, 0, SEEK_END);
  
  system_call(SYS_LSEEK, fileDesc, 0x291, SEEK_SET);
  system_call(SYS_WRITE, fileDesc, xName, strlen(xName));
  system_call(SYS_WRITE, fileDesc, ".", 1);
  system_call(SYS_WRITE, fileDesc, "\n", 1);
  if (leftToDelete > 0){
    while (leftToDelete--){
      system_call(SYS_WRITE, fileDesc, &clean, 1);
    }
  }

  system_call(SYS_CLOSE,fileDesc);
  
  return 0;
}
