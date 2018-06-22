#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include "line_parser.h"
#include "job_control.h"

#ifndef NULL
#define NULL 0
#endif

#define FREE(X) if(X) free((void*)X)

int execute(cmd_line *line, int leftPipe[], int rightPipe[], job *job1, job** job_list,pid_t pid,struct termios* termios1) {
    pid_t childPid = fork();

    if (childPid == 0) {
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        if(job1->pgid == 0)
            job1->pgid = childPid;

        setpgid(0,job1->pgid);

        if (line->input_redirect) {
            close(STDIN_FILENO);
            FILE *inputFile = fopen(line->input_redirect, "r+");
            if (!inputFile) {
                perror("Open failed");
                free_cmd_lines(line);
                _exit(1);
            }
        }

        if (line->output_redirect) {
            close(STDOUT_FILENO);
            FILE *outputFile = fopen(line->output_redirect, "r+");
            if (!outputFile) {
                fopen(line->output_redirect, "w+");
            }
        }

        if (rightPipe) {
            close(STDOUT_FILENO);
            dup(rightPipe[1]);
        }

        if (leftPipe) {
            close(STDIN_FILENO);
            dup(leftPipe[0]);
        }

        if (execvp(line->arguments[0], line->arguments) == -1) {
            perror("Error");
            free_cmd_lines(line);
            return -1;
        }

        exit(0);
    } else if (childPid > 0) {
        if(job1->pgid == 0)
            job1->pgid = childPid;

        if (rightPipe) {
            close(rightPipe[1]);
        }
        if (leftPipe) {
            close(leftPipe[0]);
        }

        if (line->blocking)
            run_job_in_foreground(job_list, job1, 0, termios1, getpid());
        else
            run_job_in_background(job1,0);

        if (line->next) {
            if (line->next->next) {
                int nextRightPipe[2];
                if (pipe(nextRightPipe) == -1) {
                    perror("Pipe");
                    exit(EXIT_FAILURE);
                }
                if (execute(line->next, rightPipe, nextRightPipe,job1, job_list,pid,termios1) == -1) {
                    perror("Execute");
                    exit(EXIT_FAILURE);
                }
            } else {
                if (execute(line->next, rightPipe, NULL,job1,job_list,pid,termios1) == -1) {
                    perror("Execute");
                    exit(EXIT_FAILURE);
                }
            }
        }

    } else {
        perror("Fork");
        return -1;
    }
    return 0;
}

void signalHandler(int sigNumber) {
    printf("Caught signal number %d.\nIt's description is: %s.\nSignal was ignored.\n", sigNumber,
           strsignal(sigNumber));
}

void printCW(char *currDirBuffer) {
    getcwd(currDirBuffer, PATH_MAX);
    printf("%s$:", currDirBuffer);
}

int main(int argc, char *argv[]) {
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGQUIT, signalHandler);
    signal(SIGCHLD, signalHandler);
    setpgid(0,0);  //Sets the process group of the shell to its process id.
    struct termios* termios1 = malloc(sizeof(struct termios));
    tcgetattr(STDIN_FILENO,termios1); //Save default terminal attributes to restore them back when a process running in the foreground ends.
    char currDirBuffer[PATH_MAX];
    char inputBuffer[2048];
    cmd_line *cmdLine;

    job** job_list = malloc(sizeof(job *));

    while (1) {
        printCW(currDirBuffer);
        if (fgets(inputBuffer, 2048, stdin) != NULL) {
            cmdLine = parse_cmd_lines(inputBuffer);
            if (strcmp(inputBuffer, "quit\n") == 0) {
                free_cmd_lines(cmdLine);
                break;
            }
            else if (!strcmp(inputBuffer, "\n")) {}
            else if (strcmp(inputBuffer, "jobs\n") == 0) {
                print_jobs(job_list);
            }else if(strcmp(cmdLine->arguments[0],"fg") == 0){
                job* job1 = find_job_by_index(job_list,atoi(cmdLine->arguments[1]));
                run_job_in_foreground(job_list, job1, 1, termios1, getpid());
            }else if(strcmp(cmdLine->arguments[0],"bg") == 0){
                job* job2 = find_job_by_index(job_list,atoi(cmdLine->arguments[1]));
                run_job_in_background(job2,1);
            }
            else if (!cmdLine->next) {
                job* job1 = add_job(job_list, inputBuffer);
                job1->status = RUNNING;
                if(execute(cmdLine, NULL, NULL,job1,job_list,getpid(),termios1) == -1){
                    free_cmd_lines(cmdLine);
                    exit(EXIT_FAILURE);
                }
            } else {
                int rightPipe[2];
                if (pipe(rightPipe) == -1) {
                    perror("Pipe");
                    exit(EXIT_FAILURE);
                }
                job* job1 = add_job(job_list, inputBuffer);
                job1->status = RUNNING;
                if (execute(cmdLine, NULL, rightPipe,job1,job_list,getpid(),termios1) == -1) {
                    free_cmd_lines(cmdLine);
                    exit(EXIT_FAILURE);
                }
            }
            free_cmd_lines(cmdLine);
        }
    }
    free(termios1);
    free_job_list(job_list);
    return 0;
}

static char *clone_first_word(char *str) {
    char *start = NULL;
    char *end = NULL;
    char *word;

    while (!end) {
        switch (*str) {
            case '>':
            case '<':
            case 0:
                end = str - 1;
                break;
            case ' ':
                if (start)
                    end = str - 1;
                break;
            default:
                if (!start)
                    start = str;
                break;
        }
        str++;
    }

    if (start == NULL) {
        return NULL;
    }

    word = (char *) malloc(end - start + 2);
    strncpy(word, start, ((int) (end - start) + 1));
    word[(int) ((end - start) + 1)] = 0;

    return word;
}

static void extract_redirections(char *str_line, cmd_line *line) {
    char *s = str_line;

    while ((s = strpbrk(s, "<>"))) {
        if (*s == '<') {
            FREE(line->input_redirect);
            line->input_redirect = clone_first_word(s + 1);
        } else {
            FREE(line->output_redirect);
            line->output_redirect = clone_first_word(s + 1);
        }

        *s++ = 0;
    }
}

static char *str_clone(const char *source) {
    char *clone = (char *) malloc(strlen(source) + 1);
    strcpy(clone, source);
    return clone;
}

static int is_empty(const char *str) {
    if (!str)
        return 1;

    while (*str) {
        if (!isspace(*(str++))) {
            return 0;
        }
    }
    return 1;
}

static cmd_line *parse_single_cmd_line(const char *str_line) {
    char *delimiter = " ";
    char *line_cpy, *result;

    if (is_empty(str_line)) {
        return NULL;
    }

    cmd_line *line = (cmd_line *) malloc(sizeof(cmd_line));
    memset(line, 0, sizeof(cmd_line));

    line_cpy = str_clone(str_line);

    extract_redirections(line_cpy, line);

    result = strtok(line_cpy, delimiter);
    while (result && line->arg_count < MAX_ARGUMENTS - 1) {
        ((char **) line->arguments)[line->arg_count++] = str_clone(result);
        result = strtok(NULL, delimiter);
    }

    FREE(line_cpy);
    return line;
}

static cmd_line *_parse_cmd_lines(char *line_str) {
    char *next_str_cmd;
    cmd_line *line;
    char pipe_delimiter = '|';

    if (is_empty(line_str)) {
        return NULL;
    }

    next_str_cmd = strchr(line_str, pipe_delimiter);
    if (next_str_cmd) {
        *next_str_cmd = 0;
    }

    line = parse_single_cmd_line(line_str);
    if (!line) {
        return NULL;
    }

    if (next_str_cmd)
        line->next = _parse_cmd_lines(next_str_cmd + 1);

    return line;
}

cmd_line *parse_cmd_lines(const char *str_line) {
    char *line, *ampersand;
    cmd_line *head, *last;
    int idx = 0;

    if (is_empty(str_line)) {
        return NULL;
    }

    line = str_clone(str_line);
    if (line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = 0;
    }

    ampersand = strchr(line, '&');
    if (ampersand) {
        *(ampersand) = 0;
    }

    if ((last = head = _parse_cmd_lines(line))) {
        while (last->next) {
            last = last->next;
        }
        last->blocking = ampersand ? 0 : 1;
    }

    for (last = head; last; last = last->next) {
        last->idx = idx++;
    }

    FREE(line);
    return head;
}


void free_cmd_lines(cmd_line *line) {
    int i;
    if (!line)
        return;

    FREE(line->input_redirect);
    FREE(line->output_redirect);
    for (i = 0; i < line->arg_count; ++i)
        FREE(line->arguments[i]);

    if (line->next)
        free_cmd_lines(line->next);

    FREE(line);
}

int replace_cmd_arg(cmd_line *line, int num, const char *new_string) {
    if (num >= line->arg_count)
        return 0;

    FREE(line->arguments[num]);
    ((char **) line->arguments)[num] = str_clone(new_string);
    return 1;
}
