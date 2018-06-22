#include "lab4_util.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_WRITE 1
#define SYS_LSEEK 8
#define SYS_GETDENTS 78
#define SYS_EXEC 59
#define SYS_VFORK 58
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define BUF_SIZE 1024

#define PRINT_FD(stdio) (printf("%d\n",(stdio)->_fileno));

extern int system_call();

void simple_system(char *command) {

    int pid = system_call(SYS_VFORK);
    if (0 == pid) {
        char *shell = "/bin/sh";
        char *cmd[] = {shell, "-c", 0, 0};
        cmd[2] = command;
        system_call(SYS_EXEC, shell, cmd, 0);
    }
}


int simple_strncmp(const char *str1, const char *str2, unsigned int n) {
    while (n--) {
        if (*str1++ != *str2++) {
            return *(unsigned char *) (str1 - 1) - *(unsigned char *) (str2 - 1);
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


int recurDir(char path[], int find, char nameToFind[], int work, char nameToWork[], char nameOfCommand[], int fileFound) {
    int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;

    fd = system_call(SYS_OPEN, path, 0, 0777);
    if (fd < 0)
        return 0x55;

    nread = system_call(SYS_GETDENTS, fd, buf, BUF_SIZE);
    if (nread < 0)
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
            } else if (work == 1 && simple_strcmp(nameToWork, objName) == 0) {
                fileFound = 1;
                char totalCommand[simple_strlen(newPath) + simple_strlen(nameOfCommand) + 1];
                for (int i = 0; i < simple_strlen(totalCommand);)
                    totalCommand[i++] = '\0';

                int i;
                for (i = 0; i < simple_strlen(nameOfCommand); i++)
                    totalCommand[i] = nameOfCommand[i];

                totalCommand[i] = ' ';
                i++;
                int h;
                for (h = 0; h < simple_strlen(newPath); h++) {
                    totalCommand[i] = newPath[h];
                    i++;
                }
                totalCommand[i] = '\0';

                simple_system(totalCommand);

            } else if (find == 0 && work == 0) {
                system_call(SYS_WRITE, STDOUT, newPath, simple_strlen(newPath));
                system_call(SYS_WRITE, STDOUT, "\n", simple_strlen("\n"));
            }if (d_type == 4) {
                newPath[i] = '/';
                newPath[i + 1] = '\0';
                recurDir(newPath, find, nameToFind, work, nameToWork, nameOfCommand, fileFound);
            }
        }
        bpos += d->d_reclen;
    }
    return fileFound;
}

int main(int argc, char *argv[]) {
    if (argc == 1 || argc >= 3) {
        char nameToFind[BUF_SIZE];
        char nameToWork[BUF_SIZE];
        char nameOfCommand[BUF_SIZE];
        int find = 0;
        int work = 0;
        int fileFound = 0;
        if (argc == 3 && simple_strcmp(argv[1], "-n") == 0) {

            find = 1;
            for (int i = 0; i < simple_strlen(nameToFind); i++) {
                nameToFind[i] = '\0';
            }
            int i = 0;
            for (i = 0; i < simple_strlen(argv[2]); i++)
                nameToFind[i] = argv[2][i];
            nameToFind[i] = '\0';

            recurDir("./", find, nameToFind, work, nameToWork, nameOfCommand, fileFound);

        } else if (argc >= 4 && simple_strcmp(argv[1], "-e") == 0) {
            work = 1;
            for (int i = 0; i < simple_strlen(nameToWork); i++) {
                nameToWork[i] = '\0';
            }
            int i = 0;
            for (i = 0; i < simple_strlen(argv[2]); i++)
                nameToWork[i] = argv[2][i];
            nameToWork[i] = '\0';

            for (int i = 0; i < simple_strlen(nameOfCommand); i++)
                nameOfCommand[i] = '\0';


            int y = 0;
            for (int k = 3; k < argc; ++k) {
                for (int j = 0; j < simple_strlen(argv[k]); j++) {
                    nameOfCommand[y] = argv[k][j];
                    y++;
                }
                nameOfCommand[y] = ' ';
                y++;
            }
            nameOfCommand[y] = '\0';

     //      recurDir("./", find, nameToFind, work, nameToWork, nameOfCommand, fileFound);
            recurDir("./", find, nameToFind, work, nameToWork, nameOfCommand, fileFound);

        } else if(argc == 1){
            system_call(SYS_WRITE, STDOUT, ".\n", simple_strlen(".\n"));
            recurDir("./", find, nameToFind, work, nameToWork, nameOfCommand, fileFound);
        }
        
        return 0;
    }
    return 0x55;
}