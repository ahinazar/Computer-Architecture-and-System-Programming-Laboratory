#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

    //booleans
    int isWord = 0; //boolean to ensure we're on word
    int finishedWithoutSpace = 0;
    int toPrintWordsCounter = 0;
    int toPrintCharacterCounter = 0;
    int toPrintLongestWordCounter = 0;

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
    }

    if(toPrintCharacterCounter==0 && toPrintLongestWordCounter == 0 && toPrintWordsCounter ==0){ //default mode
        toPrintWordsCounter = 1;
    }

    //counters
    int wordCounter=0; // counts number of words
    int characterCounter=0; //counts number of characters
    int longestWordCounter=0; //counts longest word length
    int localLengthOfWord=0;

    int str; // used in while loop

    while ( ( str = fgetc(stdin) ) != EOF ) { //as long as we're not at EOF
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
        fprintf(stdout, "Number of characters: %d\n", characterCounter); //print word counter
    }
    if(toPrintWordsCounter == 1) {
        fprintf(stdout, "Number of words: %d\n", wordCounter); //print word counter
    }
    if(toPrintLongestWordCounter == 1){
        fprintf(stdout, "Longest word length: %d\n", longestWordCounter); //print word counter
    }

    return 0;
}