#include "lab4_util.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_WRITE 1
#define SYS_LSEEK 8
#define SYS_GETDENTS 78
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define BUF_SIZE 1024
#define TYPE_DIR 4

#define PRINT_FD(stdio) (printf("%d\n",(stdio)->_fileno));

extern int system_call();

int simple_strncmp(const char* str1, const char* str2, unsigned int n)
{
    while(n--)
    {
        if(*str1++!=*str2++)
        {
            return *(unsigned char*)(str1 - 1) - *(unsigned char*)(str2 - 1);
        }
    }
    return 0;
}


unsigned int simple_strlen(const char *str) {
    int i = 0;
    while (str[i]) {
        ++i;
    }
    return i;
}

int simple_strcmp(const char *str1, const char *str2) {
    while (*str1 == *str2 && *str1) {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

struct linux_dirent {
    unsigned long d_ino;     /* Inode number */
    long d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char d_name[];  /* Filename (null-terminated) */
    /* length is actually (d_reclen - 2 -
    offsetof(struct linux_dirent, d_name)) */
    /*
    char           pad;       // Zero padding byte
    char           d_type;    // File type (only since Linux
    // 2.6.4); offset is (d_reclen - 1)
    */
};


int recurDir(char path[], int find, char nameToFind[]) {
    int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;

    fd = system_call(SYS_OPEN, path, 0, 0777);
    if (fd < 0)
        return 0x55;

    nread = system_call(SYS_GETDENTS, fd, buf, BUF_SIZE);
    if (nread == -1)
        return 0x55;
    if (nread == 0)
        return 0;

    char objName[BUF_SIZE];
    for (int i = 0; i < simple_strlen(objName); i++)
        objName[i] = '\0';

    for (bpos = 0; bpos < nread;) {
        d = (struct linux_dirent *) (buf + bpos);
        d_type = *(buf + bpos + d->d_reclen - 1);

        char newPath[simple_strlen(path) + simple_strlen(d->d_name) + 1];

        for (int i = 0; i < simple_strlen(newPath);)
            newPath[i++] = '\0';

        int i;
        for (i = 0; i < simple_strlen(path); i++)
            newPath[i] = path[i];

        int h;
        for (h = 0; h < simple_strlen(d->d_name); h++) {
            newPath[i] = d->d_name[h];
            i++;
        }
        newPath[i] = '\0';


        int j = 0;
        for (j = 0; j < simple_strlen(d->d_name); j++)
            objName[j] = d->d_name[j];

        objName[j] = '\0';

        if (simple_strcmp(d->d_name, ".") && simple_strcmp(d->d_name, "..")) {
            if (find == 1 && simple_strcmp(nameToFind, objName) == 0) {
                system_call(SYS_WRITE, STDOUT, newPath, simple_strlen(newPath));
                system_call(SYS_WRITE, STDOUT, "\n", simple_strlen("\n"));
            }else if( find == 0){
                system_call(SYS_WRITE, STDOUT, newPath, simple_strlen(newPath));
                system_call(SYS_WRITE, STDOUT, "\n", simple_strlen("\n"));
            }
            if (d_type == TYPE_DIR) {
                newPath[i] = '/';
                newPath[i + 1] = '\0';
                recurDir(newPath, find, nameToFind);
            }
        }
        bpos += d->d_reclen;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1 || argc == 3) {
        char nameToFind[BUF_SIZE];
        for (int i = 0; i < simple_strlen(nameToFind); i++)
            nameToFind[i] = '\0';
        int find = 0;
        if (argc == 3 && simple_strcmp(argv[1], "-n") == 0) {
            find = 1;
            int i=0;
            for (i= 0; i < simple_strlen(argv[2]); i++)
                nameToFind[i] = argv[2][i];
            nameToFind[i] = '\0';
        }
        if (find == 1) {
            recurDir("./", find, nameToFind);
        } else {
            system_call(SYS_WRITE, STDOUT, ".\n", simple_strlen(".\n"));
            recurDir("./", find, nameToFind);
        }
        return 0;
    }
    return 0x55;
}