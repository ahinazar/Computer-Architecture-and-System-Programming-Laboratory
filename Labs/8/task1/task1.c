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

struct funcsDesc{
    char *name;
    void (*func)();
};

int currFD = -1;
int debugFlag = 0;
void* mapStart = MAP_FAILED;
struct stat fdStat;

void toggleDBG(){
    if(debugFlag){
        debugFlag = 0;
        printf("Debug flag is off.\n");
    }else{
        debugFlag = 1;
        printf("Debug flag is on.\n");
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
    Elf64_Addr entry_point = header->e_entry;
    Elf64_Off sh_off = header->e_shoff;
    short shnum = header->e_shnum;
    short shent_size = header->e_shentsize;
    Elf64_Off ph_off = header->e_phoff;
    short phnum = header->e_phnum;
    short phent_size = header->e_phentsize;

    printf("Magic numbers: \t\t\t%02x %02x %02x\n",*(ident + 1),*(ident + 2),*(ident + 3));
    if (*(ident + 5) == 1)
        printf("Data encoding: \t\t\tlittle endian\n");
    else if (*(ident + 5) == 2)
        printf("Data encoding: \t\t\tbig endian\n");
    else
        printf("Data encoding: \t\t\tERROR\n");
    printf("Entry point: \t\t\t0x%06x \n",(unsigned int)entry_point); //Maybe to change to 08
    printf("Section header offset: \t\t%d\n",(int)sh_off);
    printf("Number of section headers: \t%hd\n",shnum);
    printf("Size of section header: \t%d\n",shent_size);
    printf("Program header offset: \t\t%d\n",(int)ph_off);
    printf("Number of program headers: \t%d\n",phnum);
    printf("Size of program header: \t%d\n\n",phent_size);
}

void printSectionNames(){

    if (mapStart == MAP_FAILED) {
        printf("No file opened.\n");
        return;
    }

    Elf64_Ehdr *elfHeader = mapStart;

    Elf64_Off shOff = elfHeader->e_shoff;
    short shnum = elfHeader->e_shnum;
    short shentSize = elfHeader->e_shentsize;

    void* mapSheaders = (void*)((char*)mapStart + shOff);

    int strShOff = elfHeader->e_shstrndx;
    Elf64_Shdr* strSecHead = (Elf64_Shdr*)((char*)mapSheaders + strShOff*shentSize);
    Elf64_Off strOff = strSecHead->sh_offset;
    char* strTable = (char*)mapStart + strOff;

    printf("[Nr]\tName\t\tAddress\t\tOffset\tSize\tType\n");
    for (int i = 0; i < shnum; i++) {
        Elf64_Shdr *sheader = (Elf64_Shdr*)((char*)mapSheaders + shentSize*i);
        printf("[%d]\t",i);
        printf("%s\t", (strTable + sheader->sh_name));
        if (strlen(strTable + sheader->sh_name) < 8)
            printf("\t");
        printf("%08x\t", (unsigned int)sheader->sh_addr);
        printf("%06x\t",(unsigned int)sheader->sh_offset);
        printf("%06x\t",(unsigned int)sheader->sh_size);

        printf("%d\n", sheader->sh_type); //need to see how to move it to name
    }
}

void quit(){
    close(currFD);
    exit(EXIT_SUCCESS);
}

int main(int argc,char** argv){

    struct funcsDesc menu[] = {{"Toggle Debug Mode", toggleDBG},{"Examine ELF File", examineElfFile},{"Print Section Names",printSectionNames},{"Quit", quit},{NULL,NULL}};

    char option;
    char afterOption;

    while(1){

        if(debugFlag){
            printf("Debugging\n");
        }

        printf("Choose action:\n");
        for(int i = 0; i<4;i++ )
            printf("%d-%s\n",i,menu[i].name);


        option = fgetc(stdin);
        afterOption = fgetc(stdin);

        while (afterOption != EOF && afterOption != '\n')
            afterOption = fgetc(stdin);

        if( option >= '0' && option <= '3')
            menu[option - '0'].func();
        else
            printf("Number out of bounds \n");
    }
}