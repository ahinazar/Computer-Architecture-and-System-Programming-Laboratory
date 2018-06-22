#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//-------------DEFINE--------------

#define STACK_MAX 1024

//-----------STRUCTS---------------
typedef struct bignum {
    long number_of_digits;
    char *digit;
    int is_negavite;
} bignum;

typedef struct Stack {
    bignum *data[STACK_MAX];
    int size;
} Stack;

void reallocNumber(bignum *number);

bignum *mult(bignum *number1, bignum *number2, bignum *result);

void div_numbers_helper(bignum *number1, bignum *number2, bignum *result, bignum *factor);

bignum *div_numbers(bignum *number1, bignum *number2, bignum *result, bignum *factor);
//---------------------------------------------EXTERN FUNC---------------------------------------------------------

extern bignum *mult_numbers(bignum *number1, bignum *number2, bignum *number3);

extern bignum *add_numbers(bignum *number1, bignum *number2);

extern bignum *sub_numbers(bignum *number1, bignum *number2);

extern int odd_number(char *digit);

extern bignum *div_by_two(bignum *number);

//---------------------------------------------BIGNUM FUNC---------------------------------------------------------

int numberIsOne(bignum *number) {
    if (number->digit[0] == '1') {
        for (int i = 1; i < number->number_of_digits; i++) {
            if (number->digit[i] != '0')
                return 0;
        }
        return 1;
    }
    return 0;
}

int numberIsZero(bignum *number) {
    if (number->digit[0] == '0' && number->number_of_digits == 1) {
        return 1;
    }
    return 0;
}


void initNumber(bignum *number, long increment) {
    number->is_negavite = 0;
    number->number_of_digits = 0;
    increment == 0 ? increment = 4 : 0;
    number->digit = malloc(1 * increment * sizeof(char) + 10);
}

bignum *initToNumber(bignum *number, char realNumber, long increment) {
    initNumber(number, increment);
    number->number_of_digits++;
    number->digit[0] = realNumber;
    return number;
}

void resetNumber(bignum *number) {
    number->is_negavite = 0;
    number->number_of_digits = 0;
    number->digit[0] = '\0';
}

void insertCharToNumber(bignum *number, char input) {
    number->digit[number->number_of_digits] = input;
    number->number_of_digits++;
}


void copyBigNum(bignum *stack_number, bignum *number) {
    stack_number->number_of_digits = number->number_of_digits;
    stack_number->is_negavite = number->is_negavite;
    memcpy(stack_number->digit, number->digit, number->number_of_digits);
}

bignum *bignumReverse(bignum *number) {
    char tmp;
    long end = (number->number_of_digits) - 1;
    for (long begin = 0; begin < end; begin++) {
        tmp = number->digit[end];
        number->digit[end] = number->digit[begin];
        number->digit[begin] = tmp;
        end--;
    }
    return number;
}

//-----------------------------------------------STACK FUNC-----------------------------------------------------

void stackInit(Stack *stack) {
    for (int i = 0; i < STACK_MAX; i++) {
        stack->data[i] = 0;
    }
    stack->size = 0;
}

void stackPush(Stack *stack, bignum *d) {
    if (stack->size < STACK_MAX)
        stack->data[stack->size++] = d;
}

bignum *stackPop(Stack *S) {
    if (S->size == 0) {
        return NULL;
    } else {
        bignum *ret = S->data[--S->size];
        S->data[S->size] = 0;
        return ret;
    }
}

bignum *stackPeek(Stack *S) {
    if (S->size == 0) {
        return NULL;
    }
    return S->data[S->size - 1];
}

int stackIsEmpty(Stack *S) {
    return S->size == 0;
}

void stackClear(Stack *stack) {
    stack->size = 0;
}

//------------------------------------------------GENERAL FUNC---------------------------------------------------------

long incrementSize(bignum *number) {
    if ((number->number_of_digits % 4) == 0) {
        long howMuch = 0;
        howMuch = number->number_of_digits / 4;
        return howMuch;
    }
    return 0;
}

void reallocNumber(bignum *number) {
    long increment = incrementSize(number);
    if (increment) {
        char *tmp = realloc(number->digit, 2 * (4 * increment));
        if (tmp != NULL) {
            number->digit = tmp;
        }
    }
}

void freeBignum(bignum *number) {
    if (number != NULL) {
        if (number->digit != NULL) {
            free(number->digit);
            number->digit=0;
        }
        free(number);
    }
}

bignum *duplicateNumber(bignum *number, long increment) {
    bignum *stack_number = malloc(1 * (sizeof(bignum)));
    increment == 0 ? increment = number->number_of_digits : 0;
    initNumber(stack_number, increment);
    copyBigNum(stack_number, number);
    return stack_number;
}

void duplicateResetAndPushNumber(bignum *number, Stack *stack) {
    if (number->number_of_digits > 0) {
        bignum *stack_number = duplicateNumber(number, 0);
        stackPush(stack, stack_number);
        resetNumber(number);
        initNumber(number, 1);
    }
}

void printTopNumber(Stack *stack) {
    int firstNotZero = 0;
    bignum *number = stackPeek(stack);

    if (number->is_negavite && !numberIsZero(number))
        printf("-");

    for (int i = 0; i < number->number_of_digits; i++) {
        if (number->digit[i] != '0' && !firstNotZero) {
            firstNotZero = 1;
        }
        if (firstNotZero)
            printf("%c", number->digit[i]);
    }
    if (!firstNotZero)
        printf("%c", number->digit[0]);
    printf("\n");
}
//---------------------------------------------CHECKING FUNC---------------------------------------------------------

int firstIsBiggerOrEqual(bignum *number1, bignum *number2) {
    if (number1->number_of_digits > 1) {
        for (int i = number1->number_of_digits - 1; i >= 0; i--) {
            if (number1->digit[i] == '0')
                number1->number_of_digits--;
            else break;
        }
    }
    if (number2->number_of_digits > 1) {
        for (int i = number2->number_of_digits - 1; i >= 0; i--) {
            if (number2->digit[i] == '0')
                number2->number_of_digits--;
            else break;
        }
    }
    if (number1->number_of_digits < number2->number_of_digits) {
        return 0;
    }

    if (number1->number_of_digits == number2->number_of_digits) {
        for (int i = number1->number_of_digits - 1; i >= 0; i--) {
            if (number1->digit[i] != number2->digit[i]) {
                if (number1->digit[i] < number2->digit[i])
                    return 0;
                else
                    break;
            }
        }
    }
    return 1;
}

void copyContant(bignum *number1, bignum *number2) {
    for (int i = 0; i < number2->number_of_digits; i++) {
        number1->digit[i] = number2->digit[i];
    }
    number1->number_of_digits = number2->number_of_digits;
    number1->is_negavite = number2->is_negavite;
}

void orgOrderOfNumbers(bignum *number1, bignum *number2, int switchSign) {
    if (!firstIsBiggerOrEqual(number1, number2)) {
        bignum *oldNumber1 = duplicateNumber(number1, 0);
        copyContant(number1, number2); 
        copyContant(number2, oldNumber1);
        if (switchSign) {
            if (number1->is_negavite)
                number1->is_negavite = 0;
            else
                number1->is_negavite = 1;
        }
    }
}

bignum *checkAndSendToAdd(bignum *number1, bignum *number2) {
    if (number1->is_negavite == 0 && number2->is_negavite == 0) {
        orgOrderOfNumbers(number1, number2, 0);
        return add_numbers(number1, number2);
    } else if (number1->is_negavite == 1 && number2->is_negavite == 1) {
        orgOrderOfNumbers(number1, number2, 0);
        return add_numbers(number1, number2);
    } else if (number1->is_negavite == 0 && number2->is_negavite == 1) {
        orgOrderOfNumbers(number1, number2, 0);
        return sub_numbers(number1, number2);
    } else {
        orgOrderOfNumbers(number1, number2, 0);
        return sub_numbers(number1, number2);
    }
}

bignum *checkAndSendToSub(bignum *number1, bignum *number2) {
    if (number1->is_negavite == 0 && number2->is_negavite == 0) {
        orgOrderOfNumbers(number1, number2, 1);
        return sub_numbers(number1, number2);
    } else if (number1->is_negavite == 1 && number2->is_negavite == 1) {
        orgOrderOfNumbers(number1, number2, 1);
        return sub_numbers(number1, number2);
    } else if (number1->is_negavite == 0 && number2->is_negavite == 1) {
        orgOrderOfNumbers(number1, number2, 1);
        return add_numbers(number1, number2);
    } else {
        orgOrderOfNumbers(number1, number2, 1);
        return add_numbers(number1, number2);
    }
}

bignum *checkAndSendToMult(bignum *number1, bignum *number2, bignum *result) {
    if (number1->is_negavite == 0 && number2->is_negavite == 0) {
        orgOrderOfNumbers(number1, number2, 0);
        number1 = mult(number1, number2, result);
        return number1;
    } else if (number1->is_negavite == 1 && number2->is_negavite == 1) {
        orgOrderOfNumbers(number1, number2, 0);
        number1 = mult(number1, number2, result);
        number1->is_negavite = 0;
        return number1;
    } else if (number1->is_negavite == 0 && number2->is_negavite == 1) {
        orgOrderOfNumbers(number1, number2, 0);
        number1 = mult(number1, number2, result);
        number1->is_negavite = 1;
        return number1;
    } else {
        orgOrderOfNumbers(number1, number2, 0);
        number1 = mult(number1, number2, result);
        number1->is_negavite = 1;
        return number1;
    }
}

bignum *firstIsSmallerDiv(bignum *number) {
    number->digit[0] = '0';
    number->number_of_digits = 1;
    number->is_negavite = 0;
    return number;
}

bignum *checkAndSendToDiv(bignum *number1, bignum *number2, bignum *result, bignum *factor) {
    if (!firstIsBiggerOrEqual(number1, number2)) {
        return firstIsSmallerDiv(number1);
    }
    if (number1->is_negavite == 0 && number2->is_negavite == 0) {
        orgOrderOfNumbers(number1, number2, 0);
        return div_numbers(number1, number2, result, factor);
    } else if (number1->is_negavite == 1 && number2->is_negavite == 1) {
        orgOrderOfNumbers(number1, number2, 0);
        result = div_numbers(number1, number2, result, factor);
        result->is_negavite = 0;
        freeBignum(number1);
        return result;
    } else if (number1->is_negavite == 0 && number2->is_negavite == 1) {
        orgOrderOfNumbers(number1, number2, 0);
        result = div_numbers(number1, number2, result, factor);
        result->is_negavite = 1;
        freeBignum(number1);
        return result;
    } else {
        orgOrderOfNumbers(number1, number2, 0);
        result = div_numbers(number1, number2, result, factor);
        result->is_negavite = 1;
        freeBignum(number1);
        return result;
    }
}

//---------------------------------------------ARITMETIC FUNC---------------------------------------------------------

void printnum(bignum *number) {
    for (int i = 0; i < number->number_of_digits; i++) {
        printf("%c", number->digit[i]);
    }
    printf("\n");
}

bignum *mult(bignum *number1, bignum *number2, bignum *result) {
    if (numberIsZero(number2)) {
        bignum *oldNumber1 = number1;
        number1 = duplicateNumber(number2, 0);
        freeBignum(oldNumber1);
        return number1;
    }
    if (numberIsOne(number2)) {
        number1->number_of_digits++;
        reallocNumber(number1);
        number1->number_of_digits--;
        number1 = add_numbers(number1, result);
        return number1;
    }

    if (odd_number(number2->digit)) {
        bignum *dupNumber1 = duplicateNumber(number1, (number1->number_of_digits + number2->number_of_digits));
        bignum *oldResult = result;
        result->number_of_digits++;
        reallocNumber(result);
        result->number_of_digits--;
        result = add_numbers(dupNumber1, result);
        freeBignum(oldResult);
    }

    bignum *dupNumber1 = duplicateNumber(number1, 0);
    number1->number_of_digits++;
    reallocNumber(number1);
    number1->number_of_digits--;
    number1 = add_numbers(number1, dupNumber1);
    number2 = bignumReverse(div_by_two(bignumReverse(number2)));

    number1 = mult(number1, number2, result);
    return number1;
}

bignum *sumResultAndFactor(bignum *result, bignum *factor) {
    if (!firstIsBiggerOrEqual(result, factor)) {
        bignum *oldResult = duplicateNumber(result, 0);
        copyContant(result, factor);
        result->number_of_digits++;
        reallocNumber(result);
        result->number_of_digits--;
        result = add_numbers(result, oldResult); 
    } else {
        result->number_of_digits++;
        reallocNumber(result);
        result->number_of_digits--;
        result = add_numbers(result, factor); 
    }
    return result;
}


void div_numbers_helper(bignum *number1, bignum *number2, bignum *result, bignum *factor) {
    if (!firstIsBiggerOrEqual(number1, number2)) {
        number2 = bignumReverse(div_by_two(bignumReverse(number2)));
        factor = bignumReverse(div_by_two(bignumReverse(factor)));
        return;
    }

    bignum *dupNumberFactor = duplicateNumber(factor, 0);
    factor->number_of_digits++;
    reallocNumber(factor);
    factor->number_of_digits--;
    factor = add_numbers(factor, dupNumberFactor);
    freeBignum(dupNumberFactor);

    bignum *dupNumber2 = duplicateNumber(number2, 0);
    number2->number_of_digits++;
    reallocNumber(number2);
    number2->number_of_digits--;
    number2 = add_numbers(number2, dupNumber2);
    freeBignum(dupNumber2);

    div_numbers_helper(number1, number2, result, factor);

    if (firstIsBiggerOrEqual(number1, number2)) {
        number1 = sub_numbers(number1, number2);
        result = sumResultAndFactor(result, factor);
    }

    number2 = bignumReverse(div_by_two(bignumReverse(number2)));
    factor = bignumReverse(div_by_two(bignumReverse(factor)));
    return;
}

bignum *div_numbers(bignum *number1, bignum *number2, bignum *result, bignum *factor) {
    div_numbers_helper(number1, number2, result, factor);
    if (firstIsBiggerOrEqual(number1, number2)) {
        number1 = sub_numbers(number1, number2);
        sumResultAndFactor(result, factor);
    }
    return result;
}



//---------------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv) {
    int input;
    bignum *number1;
    bignum *number2;
    bignum *number = malloc(1 * sizeof(bignum));
    Stack *stack = malloc(1 * sizeof(Stack));
    FILE *file = stdin;

    stackInit(stack);
    initNumber(number, 1);

    if (argc == 2) {
        file = fopen(argv[1], "r");
    }

    while ((input = getc(file)) != EOF && input != 'q') {

        if ('0' <= input && input <= '9') {
            reallocNumber(number);
            insertCharToNumber(number, input);
        } else if (input == '\n' || input == ' ') {
            duplicateResetAndPushNumber(number, stack);
        } else if (input == '_') {
            number->is_negavite = 1;
        } else if (input == '/' || input == '*' || input == '+' || input == '-') {
            duplicateResetAndPushNumber(number, stack);
            number2 = stackPop(stack);
            number1 = stackPop(stack);
            if (input == '+') {
                number1->number_of_digits++;
                reallocNumber(number1);
                number1->number_of_digits--;
                stackPush(stack, bignumReverse(checkAndSendToAdd(bignumReverse(number1), bignumReverse(number2))));
            } else if (input == '-') {
                stackPush(stack, bignumReverse(checkAndSendToSub(bignumReverse(number1), bignumReverse(number2))));
            } else if (input == '*') {
                bignum *result = malloc(1 * sizeof(bignum));
                initNumber(result, (number1->number_of_digits + number2->number_of_digits));
                result->number_of_digits++;
                result->digit[0] = '0';

                stackPush(stack,
                          bignumReverse(checkAndSendToMult(bignumReverse(number1), bignumReverse(number2), result)));
            } else {
                if (!numberIsZero(number2)) {
                    bignum *result = malloc(1 * sizeof(bignum)); 
                    bignum *factor = malloc(1 * sizeof(bignum));
                    result = initToNumber(result, '0', number1->number_of_digits);
                    factor = initToNumber(factor, '1', number1->number_of_digits);
                    stackPush(stack,
                              bignumReverse(
                                      checkAndSendToDiv(bignumReverse(number1), bignumReverse(number2), result,
                                                        factor)));
                }
            }
            freeBignum(number2);

        } else if (input == 'p') {
            duplicateResetAndPushNumber(number, stack);
            if (!stackIsEmpty(stack))
                printTopNumber(stack);
        } else if (input == 'c' && !stackIsEmpty(stack)) {
            duplicateResetAndPushNumber(number, stack);
            stackClear(stack);
        } else
            duplicateResetAndPushNumber(number, stack);
    }
        for (int i = 0; i < STACK_MAX; i++) {
        free(stack->data[i]);
        stack->data[i]=NULL;
    }
    
    freeBignum(number);
    free(stack);

    if (file != stdin) {
        fclose(file);
    }

    return 0;
}
