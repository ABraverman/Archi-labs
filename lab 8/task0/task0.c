#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/mman.h>

#define EI_DATA         5               /* Data encoding byte index */
#define ELFDATANONE     0               /* Invalid data encoding */
#define ELFDATA2LSB     1               /* 2's complement, little endian */
#define ELFDATA2MSB     2               /* 2's complement, big endian */
#define ELFDATANUM      3




typedef struct {
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    int Currentfd;
    void *map_start; /* will point to the start of the memory mapped file */
    struct stat fd_stat; /* this is needed to  the size of the file */
    Elf32_Ehdr *header; /* this will point to the header structure */
    int num_of_section_headers;

  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

// struct stat {
//                dev_t     st_dev;         /* ID of device containing file */
//                ino_t     st_ino;         /* Inode number */
//                mode_t    st_mode;        /* File type and mode */
//                nlink_t   st_nlink;       /* Number of hard links */
//                uid_t     st_uid;         /* User ID of owner */
//                gid_t     st_gid;         /* Group ID of owner */
//                dev_t     st_rdev;        /* Device ID (if special file) */
//                off_t     st_size;        /* Total size, in bytes */
//                blksize_t st_blksize;     /* Block size for filesystem I/O */
//                blkcnt_t  st_blocks;      /* Number of 512B blocks allocated */

//                /* Since Linux 2.6, the kernel supports nanosecond
//                   precision for the following timestamp fields.
//                   For the details before Linux 2.6, see NOTES. */

//                struct timespec st_atim;  /* Time of last access */
//                struct timespec st_mtim;  /* Time of last modification */
//                struct timespec st_ctim;  /* Time of last status change */

//            #define st_atime st_atim.tv_sec      /* Backward compatibility */
//            #define st_mtime st_mtim.tv_sec
//            #define st_ctime st_ctim.tv_sec

// };




struct fun_desc {
  char *name;
  void (*fun)(state*);
};

void dbug_mode(state* s);
void ex_elf(state* s);
void quit(state* s);

char* encoding_convert(int n);

 
int main(int argc, char **argv){

    struct fun_desc menu[] = { { "Toggle Debug Mode", dbug_mode }, { "Examine ELF File", ex_elf }, { "Quit", quit }, { NULL, NULL } };
    char tmpStr[100];
    int opNum = -1;
    int bound = 0;
    state* s = calloc(1, sizeof(state));
    s->unit_size = 1;
    s->Currentfd = -1;

    while ((menu+bound)->name != NULL){
        bound++;
    }
    bound -= 1;

    while (1){

        printf("Please choose a function:\n");
        for (int i = 0; (menu + i)->name != NULL; i++) {
            printf("%d) %s \n", i, (menu + i)->name);
        }
        printf("option: ");
    
        fgets(tmpStr, 100, stdin);
        if (strcmp(tmpStr, "\n") == 0){
            continue;
        }
        opNum = -1;
        sscanf(tmpStr, "%d", &opNum);

        if ( opNum < 0 || opNum > bound){
            printf("Not within bounds\n");
            quit(s);
        }
        
        printf("%d\n", opNum);
        printf("Within bounds\n");
                
        (menu + opNum)->fun(s);
        printf("DONE.\n\n");
    }
}

void dbug_mode(state* s){
  s->debug_mode = 1 - s->debug_mode;
  printf("Debug flag now %s \n", (s->debug_mode ? "on" : "off"));
}

void ex_elf(state* s){
     
    printf("Please enter a file name: ");
    fgets(s->file_name, 100, stdin);
    strtok(s->file_name, "\n");

    if (s->Currentfd >= 0){
        close(s->Currentfd);
        s->Currentfd = -1;
    }





    if( (s->Currentfd = open(s->file_name, O_RDWR)) < 0 ) {
        perror("error in open");
    }

    if( fstat(s->Currentfd, &(s->fd_stat)) != 0 ) {
        perror("stat failed");
        close(s->Currentfd);
        s->Currentfd = -1;
    }

    if ( (s->map_start = mmap(0, s->fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, s->Currentfd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
        close(s->Currentfd);
        s->Currentfd = -1;
    }

    s->header = (Elf32_Ehdr *) s->map_start;

    s->num_of_section_headers = s->header->e_shnum;

    if (strncmp((char*)(s->header->e_ident+1), "ELF", 3) != 0)
    {
        printf("not an ELF file\n");
        munmap(s->map_start, s->fd_stat.st_size);
        close(s->Currentfd);
        s->Currentfd = -1;

    }
    else{
        printf("ELF Header\n\tELF magic: %.4s\n\tData encoding: %s\n\tEntry point: %#x\n\tSection header offset: %d (bytes into file)\n\tNumber of section headers: %d\n\tSection header size: %d\n\tProgram header offset: %d (bytes into file)\n\tNumber of program headers: %d\n\tProgram header size: %d\n\t", s->header->e_ident,encoding_convert(s->header->e_ident[5]), s->header->e_entry, s->header->e_shoff, s->header->e_shnum, s->header->e_shentsize, s->header->e_phoff, s->header->e_phnum, s->header->e_phentsize);
    }
    


    if (s->debug_mode){
        fprintf(stderr, "\nDebug: file name set to: %s file descriptor: %d\n\n", s->file_name, s->Currentfd);
    }

}



void quit(state* s){
    if (s->debug_mode){
        fprintf(stderr, "Quitting\n\n");
    }
    exit(0);
}


char* encoding_convert(int n){
    switch (n)
    {
        case ELFDATANONE:
            return "Invalid data encoding";
            break;
        case ELFDATA2LSB:
            return "2's complement, little endian";
            break;
        case 2:
            return "2's complement, big endian";
            break;
    
        default:
            return "Invalid data encoding";
            break;
    }
}
