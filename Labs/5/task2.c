#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include "line_parser.h"

int main(int argc, char *argv[]) {

    int status;
    int i = 0;
    int pipefd[2];
    pid_t child1;
    pid_t child2;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    child1 = fork();

    if (child1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0) {
        int stdout_copy = dup(1);
        close(STDOUT_FILENO);
        int dupWriteEnd = dup(pipefd[1]);
        char *args1[] = {"ls","-l", NULL};
        i = execvp(args1[0], args1);
        if (i == -1) {
            perror("execvp1");
            _exit(EXIT_FAILURE);
        }
        close(dupWriteEnd);
        dup(stdout_copy);
    }

    close(pipefd[1]);

    child2 = fork();

    if (child2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child2 == 0) {
        int stdin_copy = dup(0);
        close(STDIN_FILENO);
        int dupReadEnd = dup(pipefd[0]);
        char *args2[] = {"tail","-n","2", NULL};
        i = execvp(args2[0], args2);
        if (i == -1) {
            perror("execvp2");
            _exit(0);
        }
        close(dupReadEnd);
        dup(stdin_copy);
    }

    close(pipefd[0]);

    waitpid(child1, &status, 0);
    waitpid(child2, &status, 0);

    return 0;
}