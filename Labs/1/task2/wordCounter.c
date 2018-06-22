#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

    //booleans
    int isWord = 0; //boolean to ensure we're on word
    int finishedWithoutSpace = 0;
    int toPrintWordsCounter = 0;
    int toPrintCharacterCounter = 0;
    int toPrintLongestWordCounter = 0;
    int toPrintNumberOfLinesCounter = 0;
    int toUploadInputFile = 0;

    FILE* fp;

    for(int i=0;i<argc;i++){
        if(strcmp(argv[i], "-w") == 0) {
            toPrintWordsCounter = 1;
        }
        if(strcmp(argv[i], "-c") == 0) {
            toPrintCharacterCounter = 1;
        }
        if(strcmp(argv[i], "-l") == 0) {
            toPrintLongestWordCounter = 1;
        }
        if(strcmp(argv[i], "-n") == 0) {
            toPrintNumberOfLinesCounter = 1;
        }
        if(strcmp(argv[i], "-i") == 0) {
            toUploadInputFile=1;
            if(i<argc){
                fp = fopen(argv[++i],"r");
            }
        }
    }

    if(toPrintCharacterCounter==0 && toPrintLongestWordCounter == 0 && toPrintWordsCounter ==0){ //default mode
        toPrintWordsCounter = 1;
    }

    //counters
    int wordCounter=0; // counts number of words
    int characterCounter=0; //counts number of characters
    int longestWordCounter=0; //counts longest word length
    int localLengthOfWord=0;
    int numbersOfLinesCounter = 1;

    int str; // used in while loop

    FILE* toWorkWith;

    if(toUploadInputFile==1){
        toWorkWith = fp;
    }else{
        toWorkWith = stdin;
    }

    while ( ( str = fgetc(toWorkWith) ) != EOF ) { //as long as we're not at EOF
        switch (str) {
            case ' ':
                if(isWord==1){ // moving from word to space - add to count and state that we're on word
                    if(toPrintWordsCounter == 1){
                        wordCounter++;
                    }
                    isWord=0;
                    if(toPrintLongestWordCounter == 1 && (longestWordCounter<localLengthOfWord)){
                        longestWordCounter = localLengthOfWord;
                    }
                    localLengthOfWord=0;
                }
                finishedWithoutSpace = 0;
                break;
            case '\n':
                if(isWord==1){ // moving from word to space - add to count and state that we're on word
                    if(toPrintWordsCounter == 1){
                        wordCounter++;
                    }
                    isWord=0;
                    if(toPrintLongestWordCounter == 1 && (longestWordCounter < localLengthOfWord)){
                        longestWordCounter = localLengthOfWord;
                    }
                    localLengthOfWord=0;
                }
                if(toPrintNumberOfLinesCounter){
                    numbersOfLinesCounter++;
                }
                finishedWithoutSpace = 0;
                break;
            case '\t':
                if(isWord==1){ // moving from word to space - add to count and state that we're on word
                    if(toPrintWordsCounter == 1){
                        wordCounter++;
                    }
                    isWord=0;
                    if(toPrintLongestWordCounter == 1 && (longestWordCounter<localLengthOfWord)){
                        longestWordCounter = localLengthOfWord;
                    }
                    localLengthOfWord=0;
                }
                finishedWithoutSpace = 0;
                break;
            case '\r':
                if(isWord==1){  // moving from word to space - add to count and state that we're on word
                    if(toPrintWordsCounter == 1){
                        wordCounter++;
                    }
                    isWord=0;
                    if(toPrintLongestWordCounter == 1 && (longestWordCounter<localLengthOfWord)){
                        longestWordCounter = localLengthOfWord;
                    }
                    localLengthOfWord=0;
                }
                finishedWithoutSpace = 0;
                break;
            default:
                isWord = 1; // ensure that we're on word
                if(toPrintLongestWordCounter == 1){
                    localLengthOfWord++;
                    if(localLengthOfWord > longestWordCounter){
                        longestWordCounter = localLengthOfWord;
                    }
                }
                if(toPrintCharacterCounter == 1){
                    characterCounter++;
                }
                finishedWithoutSpace = 1;
                break;
        }
    }

    if(finishedWithoutSpace == 1){
        wordCounter++;
    }

    fprintf(stdout,"\n");
    if(toPrintCharacterCounter == 1){
        fprintf(stdout, "Number of characters: %d\n", characterCounter); //print character counter
    }
    if(toPrintWordsCounter == 1) {
        fprintf(stdout, "Number of words: %d\n", wordCounter); //print word counter
    }
    if(toPrintLongestWordCounter == 1){
        fprintf(stdout, "Longest word length: %d\n", longestWordCounter); //print longest word counter
    }
    if(toPrintNumberOfLinesCounter == 1){
        fprintf(stdout, "Number of lines: %d\n", numbersOfLinesCounter); //print number of lines counter
    }

    if(toUploadInputFile == 1){
        fclose(toWorkWith);
    }

    return 0;
}