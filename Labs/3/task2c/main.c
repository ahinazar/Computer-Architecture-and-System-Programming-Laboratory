#include "toy_stdio.h"

int main(int argc, char *argv[]) {

//    int chars_printed = toy_printf("%x, %X\\n", 496351, 496351);
//    chars_printed += toy_printf("Welcome to \\c\\n");
//    chars_printed += toy_printf("Support for explicit\\N");
//    toy_printf("Printed %d chars\n", chars_printed);

    ////task1a
    toy_printf("\ntask 1a:\n");
    toy_printf("Decimal signed: %d\n", -1);
    toy_printf("Decimal unsigned: %u\n", -1);
    toy_printf("Hex unsigned lower: %x\n", -1);
    toy_printf("Hex unsigned UPPER: %X\n", -1);
    toy_printf("Octal unsigned: %o\n", -1);
    toy_printf("Binary unsigned: %b\n", -1);

    ////task1b
    toy_printf("\ntask 1b:\n");
    toy_printf("Unsigned value: %d\n", 15);
    toy_printf("Unsigned value: %u\n", -15);

    ////task1c
    toy_printf("\ntask 1c:\n");
    int integers_array[] = {1,2,3,4,5,-1,-2,-3,-4,-5};
    char chars_array[] = {'1','2','3','4','5','a','b','c','d','e'};
    char * strings_array[] = {"This", "is", "array", "of", "strings", "6", "7", "8","9","10"};
    int array_size = 10;
    toy_printf("%Ad\n", integers_array, array_size);
    toy_printf("Print array of Decimal unsigned: %Au\n", integers_array, array_size);
    toy_printf("Print array of Hex unsigned lower: %Ax\n", integers_array, array_size);
    toy_printf("Print array of Hex unsigned UPPER: %AX\n", integers_array, array_size);
    toy_printf("Print array of Octal unsigned: %Ao\n", integers_array, array_size);
    toy_printf("Print array of Binary unsigned: %Ab\n", integers_array, array_size);
    toy_printf("Print array of Chars: %Ac\n", chars_array, array_size);

    toy_printf("Print array of strings: %As\n", strings_array, array_size);

    ////task1d
    toy_printf("\ntask 1d:\n");
    toy_printf("Non-padded string: %s\n", "str");
    toy_printf("Right-padded string: %6s\n", "str");
    toy_printf("Left-added string: %-6s\n", "str");

    toy_printf("Non-padded string: %s\n", "str");
    toy_printf("Right-padded string: %15s\n", "str");
    toy_printf("Left-added string: %-15s\n", "str");

    toy_printf("Non-padded string: %s\n", "str");
    toy_printf("Right-padded string: %1s\n", "str");
    toy_printf("Left-added string: %-1s\n", "str");

    toy_printf("With numeric placeholders: %015d\n", -1);
    toy_printf("With numeric placeholders: %05d\n", -1);
    toy_printf("With numeric placeholders: %01d\n", -1);

}