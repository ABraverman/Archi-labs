#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/mman.h>

#define EI_DATA 5     /* Data encoding byte index */
#define ELFDATANONE 0 /* Invalid data encoding */
#define ELFDATA2LSB 1 /* 2's complement, little endian */
#define ELFDATA2MSB 2 /* 2's complement, big endian */
#define ELFDATANUM 3

typedef struct
{
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    int Currentfd;
    void *map_start;     /* will point to the start of the memory mapped file */
    struct stat fd_stat; /* this is needed to  the size of the file */
    Elf32_Ehdr *header;  /* this will point to the header structure */
    int num_of_section_headers;

    /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

struct fun_desc
{
    char *name;
    void (*fun)(state *);
};

void dbug_mode(state *s);
void ex_elf(state *s);
void print_secs(state *s);
void print_syms(state *s);
void print_reTabs(state *s);
void quit(state *s);

char *encoding_convert(int n);
char *type_convert(int n);
char *get_section_name(state *s, int n);
void print_symbol_table(state *s, Elf32_Shdr *curr);
void print_relocation_table(state *s, Elf32_Shdr *retab);

int main(int argc, char **argv)
{

    struct fun_desc menu[] = {{"Toggle Debug Mode", dbug_mode}, {"Examine ELF File", ex_elf}, {"Print Section Names", print_secs}, {"Print Symbols", print_syms}, {"Relocation Tables", print_reTabs}, {"Quit", quit}, {NULL, NULL}};
    char tmpStr[100];
    int opNum = -1;
    int bound = 0;
    state *s = calloc(1, sizeof(state));
    s->unit_size = 1;
    s->Currentfd = -1;

    while ((menu + bound)->name != NULL)
    {
        bound++;
    }
    bound -= 1;

    while (1)
    {

        printf("Please choose a function:\n");
        for (int i = 0; (menu + i)->name != NULL; i++)
        {
            printf("%d) %s \n", i, (menu + i)->name);
        }
        printf("option: ");

        fgets(tmpStr, 100, stdin);
        if (strcmp(tmpStr, "\n") == 0)
        {
            continue;
        }
        opNum = -1;
        sscanf(tmpStr, "%d", &opNum);

        if (opNum < 0 || opNum > bound)
        {
            printf("Not within bounds\n");
            quit(s);
        }

        printf("%d\n", opNum);
        printf("Within bounds\n");

        (menu + opNum)->fun(s);
        printf("DONE.\n\n");
    }
}

void dbug_mode(state *s)
{
    s->debug_mode = 1 - s->debug_mode;
    printf("Debug flag now %s \n", (s->debug_mode ? "on" : "off"));
}

void ex_elf(state *s)
{

    printf("Please enter a file name: ");
    fgets(s->file_name, 100, stdin);
    strtok(s->file_name, "\n");

    if (s->Currentfd >= 0)
    {
        close(s->Currentfd);
        s->Currentfd = -1;
    }

    if ((s->Currentfd = open(s->file_name, O_RDWR)) < 0)
    {
        perror("error in open");
    }

    if (fstat(s->Currentfd, &(s->fd_stat)) != 0)
    {
        perror("stat failed");
        close(s->Currentfd);
        s->Currentfd = -1;
    }

    if ((s->map_start = mmap(0, s->fd_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, s->Currentfd, 0)) == MAP_FAILED)
    {
        perror("mmap failed");
        close(s->Currentfd);
        s->Currentfd = -1;
    }

    s->header = (Elf32_Ehdr *)s->map_start;

    s->num_of_section_headers = s->header->e_shnum;

    if (strncmp((char *)(s->header->e_ident + 1), "ELF", 3) != 0)
    {
        printf("not an ELF file\n");
        munmap(s->map_start, s->fd_stat.st_size);
        close(s->Currentfd);
        s->Currentfd = -1;
    }
    else
    {
        printf("ELF Header\n\tELF magic: %.3s\n\tData encoding: %s\n\tEntry point: %#x\n\tSection header offset: %d (bytes into file)\n\tNumber of section headers: %d\n\tSection header size: %d\n\tProgram header offset: %d (bytes into file)\n\tNumber of program headers: %d\n\tProgram header size: %d\n\t", (s->header->e_ident) + 1, encoding_convert(s->header->e_ident[5]), s->header->e_entry, s->header->e_shoff, s->header->e_shnum, s->header->e_shentsize, s->header->e_phoff, s->header->e_phnum, s->header->e_phentsize);
    }

    if (s->debug_mode)
    {
        fprintf(stderr, "\nDebug: file name set to: %s file descriptor: %d\n\n", s->file_name, s->Currentfd);
    }
}

void print_secs(state *s)
{

    Elf32_Shdr *curr = (Elf32_Shdr *)(s->map_start + s->header->e_shoff);

    printf("Section headers:\n");
    printf("\t[Ind]\tName\t\t\tAddress\t\tOffset\tSize\tType\n");
    for (int i = 0; i < s->header->e_shnum; i++)
    {
        printf("\t%d\t%-16s\t%08x\t%d\t%d\t%s\n", i, get_section_name(s, curr->sh_name), curr->sh_addr, curr->sh_offset, curr->sh_size, type_convert(curr->sh_type));
        curr = (Elf32_Shdr *)(((void *)curr) + s->header->e_shentsize);
    }
}

void print_syms(state *s)
{

    Elf32_Shdr *curr = (Elf32_Shdr *)(s->map_start + s->header->e_shoff);

    for (int i = 0; i < s->header->e_shnum; i++)
    {
        if (curr->sh_type == SHT_SYMTAB || curr->sh_type == SHT_DYNSYM)
        {
            print_symbol_table(s, curr);
        }

        curr = (Elf32_Shdr *)(((void *)curr) + s->header->e_shentsize);
    }
}

void print_reTabs(state *s)
{

    Elf32_Shdr *curr = (Elf32_Shdr *)(s->map_start + s->header->e_shoff);

    for (int i = 0; i < s->header->e_shnum; i++)
    {
        if (curr->sh_type == SHT_REL || curr->sh_type == SHT_RELA)
        {
            print_relocation_table(s, curr);
        }

        curr = (Elf32_Shdr *)(((void *)curr) + s->header->e_shentsize);
    }
}

void quit(state *s)
{

    munmap(s->map_start, s->fd_stat.st_size);
    close(s->Currentfd);
    if (s->debug_mode)
    {
        fprintf(stderr, "Quitting\n\n");
    }
    exit(0);
}

char *encoding_convert(int n)
{
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

char *type_convert(int n)
{

    char *ret[] = {"Section header table entry unused", "Program data", "Symbol table", "String table", "Relocation entries with addends", "Symbol hash table", "Dynamic linking information", "Notes", "Program space with no data (bss)", "Relocation entries, no addends", "Reserved", "Dynamic linker symbol table", "Array of constructors", "Array of destructors", "Array of pre-constructors", "Array of pre-constructors", "Extended section indeces", "Number of defined types."};
    if (n > 19)
    {
        switch (n)
        {
        case SHT_LOOS:
            return "SHT_LOOS";
            break;
        case SHT_GNU_ATTRIBUTES:
            return "SHT_GNU_ATTRIBUTES";
            break;
        case SHT_GNU_HASH:
            return "SHT_GNU_HASH";
            break;
        case SHT_GNU_LIBLIST:
            return "SHT_GNU_LIBLIST";
            break;
        case SHT_CHECKSUM:
            return "SHT_CHECKSUM";
            break;
        case SHT_LOSUNW:
            return "SHT_LOSUNW";
            break;
        case SHT_GNU_verdef:
            return "SHT_GNU_verdef";
            break;
        case SHT_GNU_verneed:
            return "SHT_GNU_verneed";
            break;
        case SHT_GNU_versym:
            return "SHT_GNU_versym";
            break;
        case SHT_LOPROC:
            return "SHT_LOPROC";
            break;
        case SHT_HIPROC:
            return "SHT_HIPROC";
            break;
        case SHT_LOUSER:
            return "SHT_LOUSER";
            break;
        case SHT_HIUSER:
            return "SHT_HIUSER";
            break;

        default:
            return "Invalid type";
            break;
        }
    }
    else
    {
        return ret[n];
    }
}

char *get_section_name(state *s, int n)
{

    if (n >= 0 && n < SHN_LOPROC)
    {
        Elf32_Shdr *strtab = (Elf32_Shdr *)(s->map_start + s->header->e_shoff + ((s->header->e_shstrndx) * (s->header->e_shentsize)));
        return ((char *)(s->map_start) + (strtab->sh_offset) + n);
    }
    else
    {
        return "ABS";
    }
}

void print_symbol_table(state *s, Elf32_Shdr *symtab)
{

    Elf32_Sym *curr = s->map_start + symtab->sh_offset;
    Elf32_Shdr *symstr_Sec = (void *)symtab + s->header->e_shentsize;
    char *symstr = s->map_start + symstr_Sec->sh_offset;
    Elf32_Shdr *section_table = (Elf32_Shdr *)(s->map_start + s->header->e_shoff);

    printf("%s:\n", (symtab->sh_type == SHT_SYMTAB ? "Symol table" : "Dynamic symol table"));
    printf("\t[Ind]\tValue\t\tSection index\tSection name\t\tSymbol name\n");

    for (int i = 0; (int)curr < (int)((int)s->map_start + symtab->sh_offset + symtab->sh_size); i++)
    {

        printf("\t%d\t%08x\t%d\t\t%-16s\t%s\n", i, curr->st_value, curr->st_shndx, get_section_name(s, (curr->st_shndx < SHN_LOPROC ? (section_table + curr->st_shndx)->sh_name : curr->st_shndx)), (symstr + curr->st_name));
        curr++;
    }
}

void print_relocation_table(state *s, Elf32_Shdr *retab)
{

    Elf32_Rel *curr = s->map_start + retab->sh_offset;
    // Elf32_Shdr* symstr_Sec = (void*)retab + s->header->e_shentsize;
    // char* symstr = s->map_start + symstr_Sec->sh_offset;
    Elf32_Shdr *dynsym = (Elf32_Shdr *)(s->map_start + s->header->e_shoff);
    char *dynsymstr;

    for (int i = 0; i < s->header->e_shnum; i++)
    {
        if (dynsym->sh_type == SHT_DYNSYM)
        {
            break;
        }

        dynsym = (Elf32_Shdr *)(((void *)dynsym) + s->header->e_shentsize);
    }

    dynsymstr = s->map_start + ((Elf32_Shdr *)(((void *)dynsym) + s->header->e_shentsize))->sh_offset;

    printf("relocation table:\n");
    printf("\t Offset\t\t Info\t\ttype\tsym.value\tsym.name\n");

    for (int i = 0; (int)curr < (int)((int)s->map_start + retab->sh_offset + retab->sh_size); i++)
    {

        Elf32_Sym *currSym = s->map_start + dynsym->sh_offset;
        int currSym_index = ELF32_R_SYM(curr->r_info);
        currSym += currSym_index;
        char *name = dynsymstr + currSym->st_name;
        printf("\t%08x\t%08x\t %d\t %08x\t %s\n", curr->r_offset, curr->r_info, ELF32_R_TYPE(curr->r_info), currSym->st_value, name);
        curr++;
    }
}
