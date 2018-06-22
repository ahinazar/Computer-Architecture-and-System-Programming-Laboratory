//
// Created by ahiad on 08/06/18.
//

#include "elf.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <stdarg.h>

struct funcsDesc{
    char *name;
    void (*func)();
};

int currFD = -1;
int debugFlag = 0;
void* mapStart = MAP_FAILED;
struct stat fdStat;


void my_log(char *format,...) {
    if (debugFlag) {
        va_list args;
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
    }
}

void toggleDBG(){
    if(debugFlag){
        my_log("Debug flag is %s.\n","off");
        debugFlag = 0;
    }else{
        debugFlag = 1;
        my_log("Debug flag is %s.\n","on");
    }
}

void examineElfFile(){
    printf("Enter file name: \n");
    char filename[1024];
    char inputChar;
    int i = 0;
    while((inputChar = fgetc(stdin))!= EOF && i < 1024){
        if(inputChar == '\n'){
            filename[i] = '\0';
            break;
        }
        filename[i++] = inputChar;
    }

    printf("\n");

    if(currFD != -1){
        close(currFD);
        currFD = -1;
    }

    if(((currFD = open(filename, O_RDWR)) < 0)){
        perror("Error in open \n");
        exit(EXIT_FAILURE);
    }

    if( fstat(currFD, &fdStat) != 0){
        perror("Error in fstat \n");
        exit(-1);
    }
    if (mapStart != MAP_FAILED){
        munmap(mapStart, fdStat.st_size);
    }
    if ( (mapStart = mmap(0, fdStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, currFD, 0)) == MAP_FAILED ) {
        perror("Error in mmap \n");
        exit(-4);
    }

    Elf64_Ehdr* header = mapStart;
    unsigned char* ident = header->e_ident;

    printf("Magic numbers: \t\t\t%02x %02x %02x\n",*(ident + 1),*(ident + 2),*(ident + 3));

    if (*(ident + 5) == 1)
        printf("Data encoding: \t\t\tLittle endian\n");
    else if (*(ident + 5) == 2)
        printf("Data encoding: \t\t\tBig endian\n");
    else
        printf("Data encoding: \t\t\tERROR\n");

    printf("Entry point: \t\t\t0x%06x \n",(unsigned int)header->e_entry);
    printf("Section header offset: \t\t%d\n",(int)header->e_shoff);
    printf("Number of section headers: \t%hd\n",header->e_shnum);
    printf("Size of section header: \t%d\n",header->e_shentsize);
    printf("Program header offset: \t\t%d\n",(int)header->e_phoff);
    printf("Number of program headers: \t%d\n",header->e_phnum);
    printf("Size of program header: \t%d\n\n",header->e_phentsize);

    my_log("In function %s, magic nums were %02x %02x %02x.\n","examineElfFile",*(ident + 1),*(ident + 2),*(ident + 3));
}

void printSectionNames(){

    if (mapStart == MAP_FAILED) {
        printf("No file opened.\n");
        return;
    }

    Elf64_Ehdr* header = mapStart;

    void* mapsHeaders = mapStart + header->e_shoff;

    Elf64_Shdr* strSecHead = mapsHeaders + header->e_shstrndx * header->e_shentsize;
    char* strTable = mapStart + strSecHead->sh_offset;

    printf("[Nr]\tName\t\tAddress\t\tOffset\tSize\tType\n");
    for (int i = 0; i < header->e_shnum; i++) {
        Elf64_Shdr *secHeader = mapsHeaders + header->e_shentsize * i;
        printf("[%d]\t%s\t",i,(strTable + secHeader->sh_name));
        if (strlen(strTable + secHeader->sh_name) < 8)
            printf("\t");
        printf("%08x\t%06x\t%06x\t%d\n", (unsigned int)secHeader->sh_addr,(unsigned int)secHeader->sh_offset,(unsigned int)secHeader->sh_size,secHeader->sh_type);
    }

    my_log("In function %s, just printing 1,2,3... so... %d,%d,%d.\n","printSectionNames",1,2,3);
}

void printAllThingsNeeded( Elf64_Shdr* symTableHeader,Elf64_Shdr* headerStr,Elf64_Shdr* mapsHeaders,Elf64_Ehdr *header){
    Elf64_Sym *symTable = mapStart + symTableHeader->sh_offset;
    void *symStrTable = mapStart + headerStr->sh_offset;
    char *strTable = mapsHeaders[header->e_shstrndx].sh_offset + mapStart;

    printf("Symbol table '.dynsym' conatins %d entries:\n",(int)(symTableHeader->sh_size / 24));
    printf("Num:\tValue\t\t\tType\tNdx\t\t\tName\n");

    for (int i = 0; i < (symTableHeader->sh_size / 24); i++) {
        char *Ndx;
        if (symTable[i].st_shndx == SHN_ABS)
            Ndx = "ABS";
        else if (symTable[i].st_shndx == SHN_UNDEF)
            Ndx = "UND";
        else
            Ndx = &strTable[mapsHeaders[symTable[i].st_shndx].sh_name];

        char* symName = symStrTable + symTable[i].st_name;

        printf("[%d]\t%016x\t%d\t%s\t\t\t%s\n", i, (unsigned int) symTable[i].st_value, symTable[i].st_shndx, Ndx, symName);

    }
    printf("\n");
}

void printSymbols(){{
        Elf64_Ehdr *header = mapStart;
        Elf64_Shdr* mapsHeaders = mapStart + header->e_shoff;
        Elf64_Shdr* headerStr;
        Elf64_Shdr* symTableHeader;

        int toPrintSymTable = 0;
        int toPrintDynamicLinkerSymbolTable = 0;

        for(int i=0; i<header->e_shnum; i++) {
            if(mapsHeaders[i].sh_type == SHT_DYNSYM) {
                symTableHeader = &mapsHeaders[i];
                headerStr =  &(mapsHeaders[mapsHeaders[i].sh_link]);
                toPrintDynamicLinkerSymbolTable = 1;
                break;
            }
        }

        if(toPrintDynamicLinkerSymbolTable) {
            printAllThingsNeeded(symTableHeader,headerStr,mapsHeaders,header);
        }

        for(int i=0; i<header->e_shnum; i++) {
            if(mapsHeaders[i].sh_type == SHT_SYMTAB) {
                symTableHeader = &mapsHeaders[i];
                headerStr =  &(mapsHeaders[mapsHeaders[i].sh_link]);
                toPrintSymTable = 1;
                break;
            }
        }

        if(toPrintSymTable) {
            printAllThingsNeeded(symTableHeader,headerStr,mapsHeaders,header);
        }

        my_log("In function %s, just printing 4,5,6... so... %d,%d,%d.\n","printSymbols",4,5,6);
    }
}

void quit(){
    close(currFD);
    my_log("In function %s, just printing 7,8,9... so... %d,%d,%d.\n","printSymbols",7,8,9);
    exit(EXIT_SUCCESS);
}

int main(int argc,char** argv){

    struct funcsDesc menu[] = {{"Toggle Debug Mode", toggleDBG},{"Examine ELF File", examineElfFile},{"Print Section Names",printSectionNames},{"Print Symbols",printSymbols},{"Quit", quit},{NULL,NULL}};

    char option;
    char afterOption;

    while(1){

        if(debugFlag){
            printf("Debugging\n");
        }

        printf("Choose action:\n");
        for(int i = 0; i<5;i++ )
            printf("%d-%s\n",i,menu[i].name);

        option = fgetc(stdin);
        afterOption = fgetc(stdin);

        while (afterOption != EOF && afterOption != '\n')
            afterOption = fgetc(stdin);

        if( option >= '0' && option <= '4')
            menu[option - '0'].func();
        else
            printf("Number out of bounds \n");
    }
}