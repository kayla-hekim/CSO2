#include <stdlib.h>
#include <stdio.h>
#include "fork_run.h"

int main(int argc, const char *argv[]) {
    const char *argv_base[] = {
        "/bin/echo", "running", NULL
    };
    parallelwriteoutput(2, argv_base, "out.txt");

    // printf("Hi!\n");
    // writeoutput("echo 1 2 3; sleep 2; echo 5 5", "out.txt", "err.txt");
    // printf("Bye!\n");
}
