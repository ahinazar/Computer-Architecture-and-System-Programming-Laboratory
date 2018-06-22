//
// Created by ahinazar on 6/4/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int size = 1;
char filename[100];

void setFileName() {
    scanf("%s", filename);
}

void setUnitSize() {
    int unitSize;
    scanf("%d", &unitSize);

    if (unitSize == 1 || unitSize == 2 || unitSize == 4)
        size = unitSize;
    else
        printf("Error: unit size was not 1,2 or 4.\n");
}

void quit() {
    exit(EXIT_SUCCESS);
}

void fileDisplay() {
    if (strcmp(filename,"") == 0) {
        printf("Error: file name is null.\n");
        return;
    }
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: file is not exist.\n");
        return;
    }
    printf("Please enter <location> <length> \n");

    int length;
    int location;
    scanf("%x %d", &location, &length);

    fseek(file, location, SEEK_SET);

    size_t fixedSize = (size_t) size * (size_t) length;
    void *buffer = malloc(fixedSize);
    fread(buffer, (size_t) size, (size_t) length, file);
    fclose(file);

    printf("Hexadecimal Representation:\n");
    int i;
    for (i = 0; i < length; i++) {
        if (size == 1)
            printf("%04x ", *((char *) buffer + i));
        else if (size == 2)
            printf("%04x ", *((short *) buffer + i));
        else
            printf("%04x ", *((int *) buffer + i));
    }

    printf("\n");

    printf("Decimal Representation:\n");
    for (i = 0; i < length; i++) {
        if (size == 1)
            printf("%d ", *((char *) buffer + i));
        else if (size == 2)
            printf("%d ", *((short *) buffer + i));
        else
            printf("%d ", *((int *) buffer + i));
    }

    printf("\n");

    free(buffer);

}

void fileModify(){
    if (strcmp(filename,"") == 0) {
        printf("Error: file name is null.\n");
        return;
    }
    FILE *file = fopen(filename, "r+");
    if (!file) {
        printf("Error: file is not exist.\n");
        return;
    }
    printf("Please enter <location> <val> \n");
    int val;
    int location;
    scanf("%x %x", &location, &val);
    fseek(file, location, SEEK_SET);
    fwrite(&val,(size_t)size,1,file);
    fclose(file);
}

void copyFromFile(){
    printf("Please enter <src_file> <src_offset> <dst_offset> <length>\n");
    int srcOffset, dstOffset, length;
    char srcFileName[100];

    scanf("%s %x %x %d", srcFileName, &srcOffset, &dstOffset, &length);

    if (strcmp(filename,"") == 0) {
        printf("Error: file name is null.\n");
        return;
    }
    if (strcmp(srcFileName,"") == 0) {
        printf("Error: file name is null.\n");
        return;
    }
    FILE *dstFile = fopen(filename, "r+");
    if (!dstFile) {
        printf("Error: file is not exist.\n");
        return;
    }
    FILE *srcFile = fopen(srcFileName, "r+");
    if (!srcFile) {
        printf("Error: file is not exist.\n");
        return;
    }

    void *buffer = malloc((size_t) length);

    fseek(srcFile,srcOffset,SEEK_SET);
    fread(buffer,(size_t)length,1,srcFile);

    fseek(dstFile,dstOffset,SEEK_SET);
    fwrite(buffer,(size_t)length,1,dstFile);

    printf("Copied %d bytes into %s at %x from %s at %x\n",length, filename,dstOffset,srcFileName,srcOffset);

    fclose(dstFile);
    fclose(srcFile);
    
    free(buffer);
}

int main(int argc, char **argv) {

    int input;

    while (1) {

        printf("Choose action:\n");
        printf("1-Set File Name\n");
        printf("2-Set Unit Size\n");
        printf("3-File Display\n");
        printf("4-File Modify\n");
        printf("5-Copy From File\n");
        printf("6-Quit\n");

        scanf("%d", &input);
        void (*funcs[6])() = {&setFileName, &setUnitSize, &fileDisplay, &fileModify,&copyFromFile ,&quit};
        funcs[input - 1]();
    }
}
