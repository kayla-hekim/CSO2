#include "gettimings.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <1-8> with occasional > timings.txt\n", argv[0]);
        return 1;
    }

    int option = atoi(argv[1]); // convert 2nd argument to int to use in timing of 8 scenarios in gettimings.c
    if (option < 1 || option > 8) {
        fprintf(stderr, "option must be between 1 to 8 inclusive\n");
        return 1;
    }

    srand48(12345); // set seed for drand48() in case 2
    int successOrNoTimings = timeEightScenarios(option);
    return successOrNoTimings;
}
