#include "lab4_util.h"
#include <stdio.h>
#include <string.h>

#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_WRITE 1
#define SYS_LSEEK 8

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define PRINT_FD(stdio) (printf("%d\n",(stdio)->_fileno));

extern int system_call();

unsigned int simple_strlen(const char *str) {
    int i = 0;
    while (str[i]) {
        ++i;
    }
    return i;
}

int main(int argc, char *argv[]) {

    char* nameToChange = argv[2];
    int lengthNameToChange = simple_strlen(nameToChange);
    char*  fileAddress = argv[1];

    if(simple_strlen(nameToChange) > 5){
        return 0x55;
    }

    int fileDescriptor = system_call(SYS_OPEN, fileAddress, 0x002, 0777);

    if (fileDescriptor < 0)
        return 0x55;

    system_call(SYS_LSEEK, fileDescriptor, 0x1015, 0);
    system_call(SYS_WRITE, fileDescriptor, nameToChange, simple_strlen(nameToChange));

    for(int i = 0; i<5-lengthNameToChange ;i++){
        system_call(SYS_LSEEK,fileDescriptor,0x1015+lengthNameToChange+i,0);
        system_call(SYS_WRITE,fileDescriptor,"",1);
    }

    system_call(SYS_CLOSE, fileDescriptor);

    return 0;
}