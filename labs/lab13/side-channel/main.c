#include "lab.h"
#include "timing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *passphrase;
int check_passphrase_calls;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s TARGET-PASSPHRASE\n", argv[0]);
        printf("Target passphrase must be composed only of lowercase English letters.\n");
        return 0;
    }
    int length = strlen(argv[1]);
    passphrase = argv[1];
    check_passphrase_calls = 0;
    for (int i = 0; i < length; ++i) {
        if (passphrase[i] < 'a' || passphrase[i] > 'z') {
            fprintf(
                stderr, "'%c' in target passphrase is not a lowercase letter.\n",
                passphrase[i]
            );
        }
    }
    char *buffer = calloc(sizeof(char), length + 1);
    find_passphrase(buffer, length);
    int total_calls = check_passphrase_calls; 
    if (!check_passphrase(buffer)) {
        printf("found [%s], but that was not the passphrase\n", buffer);
    } else {
        printf("found passphrase [%s] after %d queries\n", buffer, total_calls);
    }
    return 0;
}
