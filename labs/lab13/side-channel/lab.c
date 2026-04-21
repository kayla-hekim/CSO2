#include "lab.h"

#include <string.h>     // for memset
#include <stdio.h>      // for printf

/* When complete, this function should fill in "buffer"
 * with a length-character \0-termianted string such that
 * check_passphrase(buffer) is true.
 *
 * The implementation we supplied tries the guesses of
 * 'a', 'b', and 'c' and prints out how long it takes
 * to check each of them.
 *
 * To do so, your implementation should rely on timing
 * how long check_passphrase takes, most likely by using
 * "measure_once" wrapper function.
 *
 * (Your implementation may not examine the memory in which
 *  the passphrase is stored in another way.)
 */
void find_passphrase(char *buffer, int length) {
    memset(buffer, 0, length);
    int result;
    buffer[0] = 'a';
    long time_for_a = measure_once(&result, buffer, check_passphrase);
    if (result == 1) {
        // 'a' is correct passphrase, done
        return;
    }
    buffer[0] = 'b';
    long time_for_b = measure_once(&result, buffer, check_passphrase);
    if (result == 1) {
        // 'b' is correct passphrase, done
        return;
    }
    buffer[0] = 'c';
    long time_for_c = measure_once(&result, buffer, check_passphrase);
    if (result == 1) {
        // 'c' is correct passphrase, done
        return;
    }
    printf("'a' took %ld cycles\n", time_for_a);
    printf("'b' took %ld cycles\n", time_for_b);
    printf("'c' took %ld cycles\n", time_for_c);
}
