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
void print_sections(state* s);
void print_symbols(state* s);
void print_rel_tabs(state* s);
void quit(state* s);

char* encoding_convert(int n);
char* print_section_name(state* s, char* str_tab, Elf32_Shdr* section_header_tab, int n);
void print_symbol_table(state* s, Elf32_Shdr* symTab_header);
void print_rel_table(state* s, Elf32_Shdr* relTab_header);

 
int main(int argc, char **argv){

    state* s = malloc(sizeof(state));
   printf("Please enter a file name: ");
    fgets(s->file_name, 100, stdin);
    strtok(s->file_name, "\n");

    if (s->Currentfd >= 0){
        close(s->Currentfd);
        s->Currentfd = -1;
    }





    if( (s->Currentfd = open(s->file_name, O_RDONLY)) < 0 ) {
        perror("error in open");
        return -1;
    }

    if( fstat(s->Currentfd, &(s->fd_stat)) != 0 ) {
        perror("stat failed");
        close(s->Currentfd);
        s->Currentfd = -1;
        return -1;
    }

    if ( (s->map_start = mmap(0, s->fd_stat.st_size, PROT_READ , MAP_SHARED, s->Currentfd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
        close(s->Currentfd);
        s->Currentfd = -1;
        return -1;
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
   
    Elf32_Phdr* program_header_tab = s->map_start + s->header->e_phoff;

    printf("Program Headers:\n");
    printf("\tType\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\tMemSiz\tFlg\tAlign\n");
    for (int i = 0; i < s->header->e_phnum; i++){
        printf("\t%d\t%#08x\t%#08x\t%#08x\t%#08x\t%#08x\t%d\t%#x\n", program_header_tab->p_type, program_header_tab->p_offset, program_header_tab->p_vaddr, program_header_tab->p_paddr, program_header_tab->p_filesz, program_header_tab->p_memsz, program_header_tab->p_flags, program_header_tab->p_align);
        program_header_tab++;
    }

    


    if (s->debug_mode){
        fprintf(stderr, "\nDebug: file name set to: %s file descriptor: %d\n\n", s->file_name, s->Currentfd);
    }

    return 1;
}
