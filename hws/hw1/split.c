#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "split.h"


// helper func: get if the char in the string input is delim in sep
int is_sep(char c, const char *sep) {
    for (int i = 0; sep[i] != '\0'; i++) {
        if (c == sep[i]) {
            return 1;
        }
    }
    return 0;
}


// takes a \0-terminated string input and a \0-terminated string listing separating characters in sep.
char **string_split(const char *input, const char *sep, int *num_words) {
    size_t length = strlen(input);
    size_t size = length + 1;

    if (length == 0) {
        char **retResult = malloc(sizeof(char *));
        if (!retResult) {
            return NULL;
        }
        retResult[0] = malloc(1);
        if (!retResult[0]) { 
            free(retResult); 
            return NULL; }
        retResult[0][0] = '\0';
        *num_words = 1;
        return retResult;
    }

    
    char* copy_input = (char*)malloc(size); // copy of input
        if (copy_input == NULL) {
        // printf("Memory allocation failed\n");
        return NULL;
    }
    strcpy(copy_input, input);
    length = strlen(copy_input); // finalizing copy_input final length & size
    size = length + 1;


    int index = 0;
    // pointer to char pointers (array of strings) - retResult, to be returned - size of copy_input
    char **retResult = malloc((length + 1) * sizeof(char *));
    if (retResult == NULL) {
        free(copy_input);
        return NULL;
    }

    
    // case: input & copy_input are just 1 char long and then if that 1 char is in sep:
    if (length == 1) { // only char is delim
        if (is_sep(copy_input[0], sep)) {
            retResult[0] = malloc(1);
            retResult[0][0] = '\0';
            retResult[1] = malloc(1);
            retResult[1][0] = '\0';
            *num_words = 2;
            free(copy_input);
            return retResult;
        }
        else { // only char is not delim
            retResult[0] = malloc(2);
            retResult[0][0] = copy_input[0];
            retResult[0][1] = '\0';
            *num_words = 1;
            free(copy_input);
            return retResult;
        }
    }

    int firstWordIdx = 0;
    // Check if 1st char of copy_input = within sep- true=put '\0' in the same place in the copy_input 
    if (is_sep(copy_input[0], sep)) {
        copy_input[0] = '\0';
        retResult[index] = malloc(1);
        retResult[index][0] = '\0';
        index++;
        firstWordIdx++;
    }


    // iterate through all chars besides the first and last to see if delim in sep - true=put '\0' in the same place in the copy_input 
    for (int i = 1; i < (int)length-1; i++) { 
        if (is_sep(copy_input[i], sep)) {
            // if the delims aren't right next to each other
            if (i > firstWordIdx) {
                // set aside memory for new word in retResult
                retResult[index] = malloc((i - firstWordIdx) + 1);
                // put the chars in new word from copy_input one by one into char array pointed to by index in retResult
                for (int wordIndex=firstWordIdx; wordIndex < i; wordIndex++) {
                    retResult[index][wordIndex - firstWordIdx] = copy_input[wordIndex];
                }
                retResult[index][i - firstWordIdx] = '\0';
                index++;
            }
            // cap off copy_input, and make new firstWordIdx to be char after delim
            copy_input[i] = '\0';
            firstWordIdx = i + 1;
        }
    }

    // Check if last char of copy_input = within sep - true=put '\0' in the same place in the copy_input 
    if (is_sep(copy_input[length - 1], sep)) {
        if ((int)(length - 1) > firstWordIdx) {
            int wordLength = (int)(length - 1) - firstWordIdx;
            retResult[index] = malloc(wordLength + 1);
            for (int j = firstWordIdx; j < (int)(length - 1); j++) {
                retResult[index][j - firstWordIdx] = copy_input[j];
            }
            retResult[index][wordLength] = '\0';
            index++;
        }
        retResult[index] = malloc(1);
        retResult[index][0] = '\0';
        index++;
    }
    else {
        retResult[index] = malloc((length - firstWordIdx) + 1);
        for (int wordIndex=firstWordIdx; wordIndex < (int) length; wordIndex++) {
            retResult[index][wordIndex - firstWordIdx] = copy_input[wordIndex];
        }
        retResult[index][((int)length - firstWordIdx)] = '\0';
        index++;
    }

    *num_words = index;
    free(copy_input);
    return retResult;
}
