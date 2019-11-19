
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>




char *f1();
char *f2();


int main (int argc , char* argv[], char* envp[]) {

    char* trial = f2();
    
    
    
    printf("%s\n", f1());
    printf("%s\n", f2());

    // trial[1] = "a";




return 0;

}


char *f1(){
    char trial[4] = "hus";
    printf("%c\n", trial[3]);
    return trial;
}
char *f2(){
    char* trial = "gid";
    return trial;
}
