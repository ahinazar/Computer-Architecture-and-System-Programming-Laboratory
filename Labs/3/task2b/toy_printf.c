/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* the states in the printf state-machine */
enum printf_state {
    st_printf_init,
    st_printf_percent,
    st_printf_default
};

struct state_result {
    int printed_chars;
    enum printf_state new_state;
};

typedef struct state_args{
    char* fs;
    va_list args;
    int* out_printed_chars;
    struct state_result stateResult;
    int usingArray;
    int *intArray;
    int sizeArray;
    int usingSpaces;
    int usingSpacesFromRight;
    int usingSpacesFromLeft;
    int usingZeroPadding;
    int usingMinusZeroPadding;
    int int_value;
    int charsCounter;
    int spaces;
    char **stringArray;
    char *string_value;
    char* charsArray;
    char char_value;
} state_args;

#define MAX_NUMBER_LENGTH 64

#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')
int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";

const char *DIGIT = "0123456789ABCDEF";
int printCharArray(char* array,int sizeArray, char *type){
    int count = 2;
    putchar('{');
    for (int i = 0; i < sizeArray; ++i) {
        count += toy_printf(type, array[i]);
        if (i < sizeArray - 1) {
            count += toy_printf(", "); }
    }
    putchar('}');
    return count;
}

int printIntArray(int *array, int sizeArray, char *type) {
    int count = 2;
    putchar('{');
    for (int i = 0; i < sizeArray; ++i) {
        count += toy_printf(type, array[i]);
        if (i < sizeArray - 1) {
            count += toy_printf(", "); }
    }
    putchar('}');
    return count;
}

int printStringArray(char **array, int sizeArray) {
    int count = 2;
    putchar('{');
    for (int i = 0; i < sizeArray; ++i) {
        count += toy_printf("\"%s\"", array[i]);
        if (i < sizeArray - 1) {
            count += toy_printf(", ");
        }
    }
    putchar('}');
    return count;
}

int print_int_helper(unsigned int n, int radix, const char *digit) {
    int result;
    if (n < radix) {
        putchar(digit[n]);
        return 1;
    } else {
        result = print_int_helper(n / radix, radix, digit);
        putchar(digit[n % radix]);
        return 1 + result;
    }
}

int print_int(int n, int radix, const char *digit) {
    if (radix < 2 || radix > 16) {
        toy_printf("Radix must be in [2..16]: Not %d\n", radix);
        exit(-1);
    }
    if (n > 0) {
        return print_int_helper(n, radix, digit);
    }
    if (n == 0) {
        putchar('0');
        return 1;
    } else if (radix == 10) {
        putchar('-');
        return 1 + print_int_helper((unsigned int) -n, radix, digit);
    } else
        return print_int_helper((unsigned int) n, radix, digit);
}

struct state_result init_state_handler(va_list args, char* fs, state_args *stateArgs){
    switch (*stateArgs->fs) {
        case '%':
            stateArgs->usingArray = 0;
            stateArgs->stateResult.new_state = st_printf_percent;
            return stateArgs->stateResult;

        default:
            putchar(*stateArgs->fs);
            ++stateArgs->stateResult.printed_chars;
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;
    }
}
struct state_result percent_state_handler(va_list args, char* fs, state_args *stateArgs){

    switch (*stateArgs->fs) {
        case '%':
            if (stateArgs->usingArray) {
                toy_printf("Unhandled format %%A%c...\n", *stateArgs->fs);
                exit(-1);
            }
            putchar('%');
            ++stateArgs->stateResult.printed_chars;
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;

        case 'd':
            if (stateArgs->usingArray) {
                stateArgs->intArray = va_arg(args, int*);
                stateArgs->sizeArray = va_arg(args, int);
                stateArgs->stateResult.printed_chars += printIntArray(stateArgs->intArray, stateArgs->sizeArray, "%d");
            } else {
                if (stateArgs->usingSpacesFromRight) {
                    stateArgs->int_value = va_arg(args, int);
                    stateArgs->charsCounter += print_int(stateArgs->int_value, 10, digit);
                    stateArgs->stateResult.printed_chars += stateArgs->charsCounter;
                    if (stateArgs->usingSpacesFromRight) {
                        stateArgs->spaces = stateArgs->spaces - stateArgs->charsCounter;
                        if (stateArgs->spaces > 0) {
                            for (int i = 0; i < stateArgs->spaces; i++) {
                                stateArgs->stateResult.printed_chars++;
                                putchar(' ');
                            }
                            stateArgs->stateResult.printed_chars++;
                            putchar('#');
                        }
                    }
                } else if (stateArgs->usingSpacesFromLeft) {
                    stateArgs->int_value = va_arg(args, int);
                    int int_value_memory = stateArgs->int_value;
                    while (stateArgs->int_value != 0) {
                        stateArgs->int_value /= 10;
                        ++stateArgs->charsCounter;
                    }
                    stateArgs->spaces = stateArgs->spaces - stateArgs->charsCounter;
                    if (stateArgs->spaces > 0) {
                        for (int i = 0; i < stateArgs->spaces; i++) {
                            stateArgs->stateResult.printed_chars++;
                            putchar(' ');
                        }
                    }
                    stateArgs->stateResult.printed_chars += print_int(int_value_memory, 10, digit);
                } else if (stateArgs->usingZeroPadding) {
                    stateArgs->int_value = va_arg(args,int);
                    if(stateArgs->int_value<0){
                        stateArgs->usingMinusZeroPadding = 1;
                    }
                    if (stateArgs->usingMinusZeroPadding) {
                        int int_value_memory = stateArgs->int_value;
                        stateArgs->charsCounter = 1;
                        while (stateArgs->int_value != 0) {
                            stateArgs->int_value /= 10;
                            ++stateArgs->charsCounter;
                        }
                        stateArgs->spaces = stateArgs->spaces - stateArgs->charsCounter;
                        putchar('-');
                        stateArgs->stateResult.printed_chars++;
                        if (stateArgs->spaces > 0) {
                            for (int i = 0; i < stateArgs->spaces; i++) {
                                stateArgs->stateResult.printed_chars++;
                                putchar('0');
                            }
                        }
                        int_value_memory = int_value_memory * (-1);
                        stateArgs->stateResult.printed_chars += print_int(int_value_memory, 10, digit);
                    } else {
                        int int_value_memory = stateArgs->int_value;
                        while (stateArgs->int_value != 0) {
                            stateArgs->int_value /= 10;
                            ++stateArgs->charsCounter;
                        }
                        stateArgs->spaces = stateArgs->spaces - stateArgs->charsCounter;
                        if (stateArgs->spaces > 0) {
                            for (int i = 0; i < stateArgs->spaces; i++) {
                                stateArgs->stateResult.printed_chars++;
                                putchar('0');
                            }
                        }
                        stateArgs->stateResult.printed_chars += print_int(int_value_memory, 10, digit);
                    }

                } else {
                    stateArgs->int_value = va_arg(args, int);
                    stateArgs->stateResult.printed_chars += print_int(stateArgs->int_value, 10, digit);
                }
            }
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;

        case 'b':
            if (stateArgs->usingArray) {
                stateArgs->intArray = va_arg(args, int*);
                stateArgs->sizeArray = va_arg(args, int);
                stateArgs->stateResult.printed_chars += printIntArray(stateArgs->intArray, stateArgs->sizeArray, "%b");
            } else {
                stateArgs->int_value = va_arg(args, int);
                stateArgs->stateResult.printed_chars += print_int(stateArgs->int_value, 2, digit);
            }
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;

        case 'o':
            if (stateArgs->usingArray) {
                stateArgs->intArray = va_arg(args, int*);
                stateArgs->sizeArray = va_arg(args, int);
                stateArgs->stateResult.printed_chars += printIntArray(stateArgs->intArray, stateArgs->sizeArray, "%o");
            } else {
                stateArgs->int_value = va_arg(args, int);
                stateArgs->stateResult.printed_chars += print_int(stateArgs->int_value, 8, digit);
            }
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;

        case 'x':
            if (stateArgs->usingArray) {
                stateArgs->intArray = va_arg(args, int*);
                stateArgs->sizeArray = va_arg(args, int);
                stateArgs->stateResult.printed_chars += printIntArray(stateArgs->intArray, stateArgs->sizeArray, "%x");
            } else {
                stateArgs->int_value = va_arg(args, int);
                stateArgs->stateResult.printed_chars += print_int(stateArgs->int_value, 16, digit);
            }
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;

        case 'X':
            if (stateArgs->usingArray) {
                stateArgs->intArray = va_arg(args, int*);
                stateArgs->sizeArray = va_arg(args, int);
                stateArgs->stateResult.printed_chars += printIntArray(stateArgs->intArray, stateArgs->sizeArray, "%X");
            } else {
                stateArgs->int_value = va_arg(args, int);
                stateArgs->stateResult.printed_chars += print_int(stateArgs->int_value, 16, DIGIT);
            }
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;

        case 's':
            if (stateArgs->usingArray) {
                stateArgs->stringArray = va_arg(args, char**);
                stateArgs->sizeArray = va_arg(args, int);
                stateArgs->stateResult.printed_chars += printStringArray(stateArgs->stringArray, stateArgs->sizeArray);
            } else {
                stateArgs->string_value = va_arg(args, char *);
                char *stringMemory;
                if (stateArgs->usingSpacesFromLeft) {
                    stringMemory = stateArgs->string_value;
                    while (*stateArgs->string_value) {
                        stateArgs->charsCounter++;
                        stateArgs->string_value++;
                    }
                }
                while (stateArgs->usingSpacesFromLeft == 0 && *stateArgs->string_value) {
                    stateArgs->charsCounter++;
                    stateArgs->stateResult.printed_chars++;
                    putchar(*stateArgs->string_value);
                    stateArgs->string_value++;
                }
                if (stateArgs->usingSpacesFromRight) {

                    stateArgs->spaces = stateArgs->spaces - stateArgs->charsCounter;
                    if (stateArgs->spaces > 0) {
                        for (int i = 0; i < stateArgs->spaces; i++) {
                            stateArgs->stateResult.printed_chars++;
                            putchar(' ');
                        }
                        stateArgs->stateResult.printed_chars++;
                        putchar('#');
                    }
                } else if (stateArgs->usingSpacesFromLeft) {
                    stateArgs->spaces = stateArgs->spaces - stateArgs->charsCounter;
                    if (stateArgs->spaces > 0) {
                        for (int i = 0; i < stateArgs->spaces; i++) {
                            stateArgs->stateResult.printed_chars++;
                            putchar(' ');
                        }
                    }
                    while (*stringMemory) {
                        stateArgs->stateResult.printed_chars++;
                        putchar(*stringMemory);
                        stringMemory++;
                    }
                }
            }
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;

        case 'c':
            if (stateArgs->usingArray) {
                stateArgs->charsArray = va_arg(args, char*);
                stateArgs->sizeArray = va_arg(args, int);
                stateArgs->stateResult.printed_chars += printCharArray(stateArgs->charsArray, stateArgs->sizeArray, "%c");
            } else {
                stateArgs->char_value = (char) va_arg(args, int);
                putchar(stateArgs->char_value);
                ++stateArgs->stateResult.printed_chars;
            }
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;

        case 'u':
            if (stateArgs->usingArray) {
                stateArgs->intArray = va_arg(args, int*);
                stateArgs->sizeArray = va_arg(args, int);
                stateArgs->stateResult.printed_chars += printIntArray(stateArgs->intArray, stateArgs->sizeArray, "%u");
            } else {
                stateArgs->int_value = va_arg(args, int);
                stateArgs->stateResult.printed_chars += print_int_helper((unsigned int) stateArgs->int_value, 10, digit);
            }
            stateArgs->stateResult.new_state=st_printf_init;
            return stateArgs->stateResult;

        case 'A':
            if (stateArgs->usingArray) {
                toy_printf("Unhandled format %%A%c...\n", *stateArgs->fs);
                exit(-1);
            }
            stateArgs->usingArray = 1;
            stateArgs->stateResult.new_state=st_printf_percent;
            return stateArgs->stateResult; // I ADDED IT NOW!!

        default:
            if (*stateArgs->fs == '-') {
                stateArgs->usingSpacesFromLeft = 1;
                stateArgs->stateResult.new_state=st_printf_percent;
            return stateArgs->stateResult;
            } else if (*stateArgs->fs >= '1' && *stateArgs->fs <= '9') {
                if (stateArgs->usingZeroPadding == 0) {
                    stateArgs->usingSpaces == 0 ? stateArgs->usingSpaces = 1 : 0;
                    stateArgs->usingSpacesFromLeft == 0 ? stateArgs->usingSpacesFromRight = 1 : 0;
                }
                stateArgs->spaces = 10 * stateArgs->spaces + *stateArgs->fs - '0';

                stateArgs->stateResult.new_state=st_printf_percent;
            return stateArgs->stateResult;

            } else if (*stateArgs->fs == '0') {
                if (stateArgs->usingSpaces == 0)
                    stateArgs->usingZeroPadding = 1;
                stateArgs->usingZeroPadding == 0 ? stateArgs->usingSpaces = 1 : 0;
                if (stateArgs->usingSpaces) {
                    stateArgs->usingSpacesFromLeft == 0 && stateArgs->usingZeroPadding == 0 ? stateArgs->usingSpacesFromRight = 1 : 0;
                }
                stateArgs->spaces = 10 * stateArgs->spaces;

                stateArgs->stateResult.new_state=st_printf_percent;
            return stateArgs->stateResult;
            } else {
                toy_printf("Unhandled format %%%c...\n", *stateArgs->fs);
                exit(-1);
            }
    }
}
struct state_result default_state_handler(va_list args, char* fs, state_args *stateArgs){
    toy_printf("toy_printf: Unknown state -- %d\n", (int) stateArgs->stateResult.new_state);
    exit(-1);
}



/* SUPPORTED:
 *   %b, %d, %o, %x, %X --
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {

    struct state_result (*oper[3])(va_list, char*, state_args *) = {init_state_handler,percent_state_handler,default_state_handler};

    state_args stateArgs;

    int chars_printed = 0;

    stateArgs.stateResult.printed_chars=0;
    stateArgs.int_value = 0;
    stateArgs.spaces = 0;
    stateArgs.usingSpaces = 0;
    stateArgs.usingSpacesFromRight = 0;
    stateArgs.usingSpacesFromLeft = 0;
    stateArgs.usingZeroPadding = 0;
    stateArgs.usingMinusZeroPadding = 0;
    stateArgs.usingArray = 0;
    stateArgs.charsCounter = 0;
    stateArgs.sizeArray = 0;
    stateArgs.intArray = 0;
    stateArgs.charsArray=0;
    stateArgs.stringArray = 0;
    stateArgs.fs=fs;
    stateArgs.stateResult.printed_chars=0;

    va_list args;

    va_start(args, fs);

    stateArgs.stateResult.new_state = st_printf_init;

    for (; *stateArgs.fs != '\0'; ++stateArgs.fs) {
        stateArgs.stateResult = (*oper[stateArgs.stateResult.new_state])(args, stateArgs.fs,&stateArgs);
        chars_printed += stateArgs.stateResult.printed_chars;
    }

    va_end(args);

    return chars_printed;
}
