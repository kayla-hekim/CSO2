#include "split.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char **argv) {
    char *sep = NULL;
    size_t total_len = 0;

    if (argc == 1) { // default to space, tab, null char in sep
        total_len = 3;
        sep = malloc(total_len);
        sep[0] = ' ';
        sep[1] = '\t';
        sep[2] = '\0';
    }
    else { // there are arguments
        for (int i = 1; i < argc; i++) {
            total_len += strlen(argv[i]);
        }
        sep = malloc(total_len + 1);
        sep[0] = '\0';
        for (int i = 1; i < argc; i++) {
            strcat(sep, argv[i]);
        }
    }

    char input[6000];
    while (fgets(input, sizeof(input), stdin) != NULL) {
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if (strcmp(input, ".") == 0) {
            break; // exit is the "." in cli input
        }

        int n = 0;
        char **words = string_split(input, sep, &n);

        for (int i = 0; i < n; i++){ 
            printf("[%s]", words[i]);
        }
        printf("\n");
        
        for (int i = 0; i < n; i++) {
            free(words[i]);
        }
        free(words);
    }

    free(sep);
    return 0;
}
