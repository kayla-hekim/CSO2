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
    // memset(buffer, 0, length);
    // int result;
    // buffer[0] = 'a';
    // long time_for_a = measure_once(&result, buffer, check_passphrase);
    // if (result == 1) {
    //     // 'a' is correct passphrase, done
    //     return;
    // }
    // buffer[0] = 'b';
    // long time_for_b = measure_once(&result, buffer, check_passphrase);
    // if (result == 1) {
    //     // 'b' is correct passphrase, done
    //     return;
    // }
    // buffer[0] = 'c';
    // long time_for_c = measure_once(&result, buffer, check_passphrase);
    // if (result == 1) {
    //     // 'c' is correct passphrase, done
    //     return;
    // }
    // printf("'a' took %ld cycles\n", time_for_a);
    // printf("'b' took %ld cycles\n", time_for_b);
    // printf("'c' took %ld cycles\n", time_for_c);

    // long best_time = -1;
    // char best_char = 'a';
    memset(buffer, 0, length);
    buffer[length] = '\0';
    int result;

    for (int i = 0; i < length; i+=1) {
        long best_time = -1;
        char best_char = 'a';

        for (char c = 'a'; c <= 'z'; c+=1) {
            buffer[i] = c;
            // long time_for_char = measure_once(&result, buffer, check_passphrase);

            // if (result == 1) {
            //     return;
            // }

            long total = 0;
            for (int k = 0; k < 20; k++) {
                total += measure_once(&result, buffer, check_passphrase);
                if (result == 1) {
                    return;
                }
            }

            long avg_time = total / 20;

            if (avg_time > best_time) {
                best_time = avg_time;
                best_char = c;
            }
        }
        // need to put string in result but dk how to initialize it anyways
        buffer[i] = best_char;
    }
}
