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
    st_printf_octal2,
    st_printf_octal3
};

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

/* SUPPORTED:
 *   %b, %d, %o, %x, %X --
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
    int chars_printed = 0;
    int int_value = 0;
    int spaces = 0;
    char *string_value;
    char char_value;
    //char octal_char;
    va_list args;
    enum printf_state state;

    int usingSpaces = 0;
    int usingSpacesFromRight = 0;
    int usingSpacesFromLeft = 0;
    int usingZeroPadding = 0;
    int usingMinusZeroPadding = 0;
    int usingArray = 0;
    int charsCounter = 0;
    int sizeArray = 0;
    int *intArray = 0;
    char* charsArray=0;
    char **stringArray = 0;

    va_start(args, fs);

    state = st_printf_init;

    for (; *fs != '\0'; ++fs) {
        switch (state) {
            case st_printf_init:
                switch (*fs) {
                    case '%':
                        usingArray = 0;
                        state = st_printf_percent;
                        break;

                    default:
                        putchar(*fs);
                        ++chars_printed;
                }
                break;

            case st_printf_percent:
                switch (*fs) {
                    case '%':
                        if (usingArray) {
                            toy_printf("Unhandled format %%A%c...\n", *fs);
                            exit(-1);
                        }
                        putchar('%');
                        ++chars_printed;
                        state = st_printf_init;
                        break;

                    case 'd':
                        if (usingArray) {
                            intArray = va_arg(args, int*);
                            sizeArray = va_arg(args, int);
                            chars_printed += printIntArray(intArray, sizeArray, "%d");
                        } else {
                            if (usingSpacesFromRight) {
                                int_value = va_arg(args, int);
                                charsCounter += print_int(int_value, 10, digit);
                                chars_printed += charsCounter;
                                if (usingSpacesFromRight) {
                                    spaces = spaces - charsCounter;
                                    if (spaces > 0) {
                                        for (int i = 0; i < spaces; i++) {
                                            chars_printed++;
                                            putchar(' ');
                                        }
                                        chars_printed++;
                                        putchar('#');
                                    }
                                }
                            } else if (usingSpacesFromLeft) {
                                int_value = va_arg(args, int);
                                int int_value_memory = int_value;
                                while (int_value != 0) {
                                    int_value /= 10;
                                    ++charsCounter;
                                }
                                spaces = spaces - charsCounter;
                                if (spaces > 0) {
                                    for (int i = 0; i < spaces; i++) {
                                        chars_printed++;
                                        putchar(' ');
                                    }
                                }
                                chars_printed += print_int(int_value_memory, 10, digit);
                            } else if (usingZeroPadding) {
                                int_value = va_arg(args,int);
                                if(int_value<0){
                                    usingMinusZeroPadding = 1;
                                }
                                if (usingMinusZeroPadding) {
                                    int int_value_memory = int_value;
                                    charsCounter = 1;
                                    while (int_value != 0) {
                                        int_value /= 10;
                                        ++charsCounter;
                                    }
                                    spaces = spaces - charsCounter;
                                    putchar('-');
                                    chars_printed++;
                                    if (spaces > 0) {
                                        for (int i = 0; i < spaces; i++) {
                                            chars_printed++;
                                            putchar('0');
                                        }
                                    }
                                    int_value_memory = int_value_memory * (-1);
                                    chars_printed += print_int(int_value_memory, 10, digit);
                                } else {
                                    int int_value_memory = int_value;
                                    while (int_value != 0) {
                                        int_value /= 10;
                                        ++charsCounter;
                                    }
                                    spaces = spaces - charsCounter;
                                    if (spaces > 0) {
                                        for (int i = 0; i < spaces; i++) {
                                            chars_printed++;
                                            putchar('0');
                                        }
                                    }
                                    chars_printed += print_int(int_value_memory, 10, digit);
                                }

                            } else {
                                int_value = va_arg(args, int);
                                chars_printed += print_int(int_value, 10, digit);
                            }
                        }
                        state = st_printf_init;
                        break;

                    case 'b':
                        if (usingArray) {
                            intArray = va_arg(args, int*);
                            sizeArray = va_arg(args, int);
                            chars_printed += printIntArray(intArray, sizeArray, "%b");
                        } else {
                            int_value = va_arg(args, int);
                            chars_printed += print_int(int_value, 2, digit);
                        }
                        state = st_printf_init;
                        break;

                    case 'o':
                        if (usingArray) {
                            intArray = va_arg(args, int*);
                            sizeArray = va_arg(args, int);
                            chars_printed += printIntArray(intArray, sizeArray, "%o");
                        } else {
                            int_value = va_arg(args, int);
                            chars_printed += print_int(int_value, 8, digit);
                        }
                        state = st_printf_init;
                        break;

                    case 'x':
                        if (usingArray) {
                            intArray = va_arg(args, int*);
                            sizeArray = va_arg(args, int);
                            chars_printed += printIntArray(intArray, sizeArray, "%x");
                        } else {
                            int_value = va_arg(args, int);
                            chars_printed += print_int(int_value, 16, digit);
                        }
                        state = st_printf_init;
                        break;

                    case 'X':
                        if (usingArray) {
                            intArray = va_arg(args, int*);
                            sizeArray = va_arg(args, int);
                            chars_printed += printIntArray(intArray, sizeArray, "%X");
                        } else {
                            int_value = va_arg(args, int);
                            chars_printed += print_int(int_value, 16, DIGIT);
                        }
                        state = st_printf_init;
                        break;

                    case 's':
                        if (usingArray) {
                            stringArray = va_arg(args, char**);
                            sizeArray = va_arg(args, int);
                            chars_printed += printStringArray(stringArray, sizeArray);
                        } else {
                            string_value = va_arg(args, char *);
                            char *stringMemory;
                            if (usingSpacesFromLeft) {
                                stringMemory = string_value;
                                while (*string_value) {
                                    charsCounter++;
                                    string_value++;
                                }
                            }
                            while (usingSpacesFromLeft == 0 && *string_value) {
                                charsCounter++;
                                chars_printed++;
                                putchar(*string_value);
                                string_value++;
                            }
                            if (usingSpacesFromRight) {

                                spaces = spaces - charsCounter;
                                if (spaces > 0) {
                                    for (int i = 0; i < spaces; i++) {
                                        chars_printed++;
                                        putchar(' ');
                                    }
                                    chars_printed++;
                                    putchar('#');
                                }
                            } else if (usingSpacesFromLeft) {
                                spaces = spaces - charsCounter;
                                if (spaces > 0) {
                                    for (int i = 0; i < spaces; i++) {
                                        chars_printed++;
                                        putchar(' ');
                                    }
                                }
                                while (*stringMemory) {
                                    chars_printed++;
                                    putchar(*stringMemory);
                                    stringMemory++;
                                }
                            }
                        }
                        state = st_printf_init;
                        break;

                    case 'c':
                        if (usingArray) {
                            charsArray = va_arg(args, char*);
                            sizeArray = va_arg(args, int);
                            chars_printed += printCharArray(charsArray, sizeArray, "%c");
                        } else {
                            char_value = (char) va_arg(args, int);
                            putchar(char_value);
                            ++chars_printed;
                        }
                        state = st_printf_init;
                        break;

                    case 'u':
                        if (usingArray) {
                            intArray = va_arg(args, int*);
                            sizeArray = va_arg(args, int);
                            chars_printed += printIntArray(intArray, sizeArray, "%u");
                        } else {
                            int_value = va_arg(args, int);
                            chars_printed += print_int_helper((unsigned int) int_value, 10, digit);
                        }
                        state = st_printf_init;
                        break;

                    case 'A':
                        if (usingArray) {
                            toy_printf("Unhandled format %%A%c...\n", *fs);
                            exit(-1);
                        }
                        usingArray = 1;
                        break;

                    default:
                        if (*fs == '-') {
                            usingSpacesFromLeft = 1;
                        } else if (*fs >= '1' && *fs <= '9') {
                            if (usingZeroPadding == 0) {
                                usingSpaces == 0 ? usingSpaces = 1 : 0;
                                usingSpacesFromLeft == 0 ? usingSpacesFromRight = 1 : 0;
                            }
                            spaces = 10 * spaces + *fs - '0';
                        } else if (*fs == '0') {
                            if (usingSpaces == 0)
                                usingZeroPadding = 1;
                            usingZeroPadding == 0 ? usingSpaces = 1 : 0;
                            if (usingSpaces) {
                                usingSpacesFromLeft == 0 && usingZeroPadding == 0 ? usingSpacesFromRight = 1 : 0;
                            }
                            spaces = 10 * spaces;
                        } else {
                            toy_printf("Unhandled format %%%c...\n", *fs);
                            exit(-1);
                        }
                }
                break;

            default:
                toy_printf("toy_printf: Unknown state -- %d\n", (int) state);
                exit(-1);
        }
    }

    va_end(args);

    return chars_printed;
}