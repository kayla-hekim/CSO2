#define _XOPEN_SOURCE 700
#include "gettimings.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>


// nanosecs
long long nsecs(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long long)t.tv_sec * 1000000000LL + t.tv_nsec;
}


// 1
__attribute__((noinline))
void emptyFunction(void) {
    __asm__("");
}


// 2
static volatile double sinkRandDouble;
void randNumGenerator(void) {
    sinkRandDouble = drand48();
}


// 3
static volatile pid_t sinkGetppid;
void doGetppid(void) {
    sinkGetppid = getppid();
}


// 4 
pid_t retInParent(void) { 
    pid_t processID = fork(); 

    if (processID == 0) {
        _exit(127);
    }

    return processID;
}



// 5
int terminateBeforeWaitpid(pid_t processID) {
    int status;
    return waitpid(processID, &status, 0);
}


// 6
pid_t forkThenTerminateBeforeWaitpid(void) { 
    pid_t processID = fork(); 

    if (processID == 0) {
        _exit(127);
    }
    if (processID < 0) {
        return -1;
    }

    int status;
    return waitpid(processID, &status, 0);
}


// 7
int runBinTrue(void) {
    const char *command = "/bin/true";
    if (access(command, X_OK) != 0) {
        command = "/usr/bin/true";
    }
    return system(command);
}


// 8
void createRemoveDir(void) {
    char path[256];
    snprintf(path, sizeof(path), "/tmp/gettimings_tmpdir_%ld", (long)getpid());

    mkdir(path, 0700);
    rmdir(path);
}



int timeEightScenarios(int option) {
    long long time = 0;

    int samplesFast = 1000;
    int runsFast = 10000; // 1–3
    int samplesSlow = 300; 
    // 4-8 runSlow is just 1 time through, so omitted


    // samples = 1000 (1-3)
    long long emptySumFast = 0;
    for (int i = 0; i < samplesFast; i++) {
        long long start1_3 = nsecs();

        for (int j = 0; j < runsFast; j++) {
            __asm__("");
        }

        long long end1_3 = nsecs();
        emptySumFast += (end1_3 - start1_3);
    }
    long long avgEmpty1_3 = emptySumFast / samplesFast;
    

    // samples = 300 (4-8)
    long long emptySumSlow = 0;
    for (int i = 0; i < samplesSlow; i++) {
        long long start4_8 = nsecs();
        __asm__(""); 
        long long end4_8 = nsecs();
        emptySumSlow += (end4_8 - start4_8);
    }
    long long avgEmpty4_8 = emptySumSlow / samplesSlow;


    switch (option) {
        case 1: {
            long long realTime = 0;
            for (int i = 0; i < samplesFast; i++) {
                long long start = nsecs();

                for (int j = 0; j < runsFast; j++) {
                    emptyFunction();
                }

                long long end = nsecs();
                time = end - start;
                realTime += time;
            }
            long long avgTime = (realTime / samplesFast) - avgEmpty1_3;
            double perCall = (double)avgTime / runsFast;
            printf("Case 1: empty function\n");
            printf("AVG_EMPTY_FAST %lld\n", avgEmpty1_3);
            printf("AVG_REAL_FAST %lld\n", realTime / samplesFast);
            printf("PER_CALL_NS %.3f\n\n", perCall);
            break;
        }

        case 2: {
            long long realTime = 0;
            for (int i = 0; i < samplesFast; i++) {
                long long start = nsecs();

                for (int j = 0; j < runsFast; j++) {
                    randNumGenerator();
                }

                long long end = nsecs();
                time = end - start;
                realTime += time;
            }
            long long avgTime = (realTime / samplesFast) - avgEmpty1_3;
            double perCall = (double)avgTime / runsFast;
            printf("Case 2: random number generator with drand48()\n");
            printf("AVG_EMPTY_FAST %lld\n", avgEmpty1_3);
            printf("AVG_REAL_FAST %lld\n", realTime / samplesFast);
            printf("PER_CALL_NS %.3f\n\n", perCall);
            break;
        }

        case 3: {
            long long realTime = 0;
            for (int i = 0; i < samplesFast; i++) {
                long long start = nsecs();

                for (int j = 0; j < runsFast; j++) {
                    doGetppid();
                }

                long long end = nsecs();
                time = end - start;
                realTime += time;
            }
            long long avgTime = (realTime / samplesFast) - avgEmpty1_3;
            double perCall = (double)avgTime / runsFast;
            printf("Case 3: perform getppid() on current process\n");
            printf("AVG_EMPTY_FAST %lld\n", avgEmpty1_3);
            printf("AVG_REAL_FAST %lld\n", realTime / samplesFast);
            printf("PER_CALL_NS %.3f\n\n", perCall);
            break;
        }

        case 4: {
            long long realTime = 0;
            int used = 0;

            for (int i = 0; i < samplesSlow; i++) {
                pid_t pid;
                long long start = nsecs();

                pid = retInParent();

                long long end = nsecs();
                if (pid <= 0) {
                    continue;
                }

                used++;
                time = end - start;
                realTime += time;

                int status;
                waitpid(pid, &status, 0);
            }
            if (used == 0) { // used is 0 meaning we did not use the terminateBeforeWaitpid due to an error - no divide by 0 error
                fprintf(stderr, "Case 4: no successful forks\n"); 
                break; 
            }

            long long avgTime = (realTime / used) - avgEmpty4_8;
            double perCall = (double)avgTime;
            printf("Case 4: perform retInParent() which forks\n");
            printf("AVG_EMPTY_SLOW %lld\n", avgEmpty4_8);
            printf("AVG_REAL_SLOW %lld\n", realTime / used);
            printf("PER_CALL_NS %.3f\n\n", perCall);
            break;
        }

        case 5: {
            long long realTime = 0;
            int used = 0;

            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 1000000;
            for (int i = 0; i < samplesSlow; i++) {
                pid_t processID = retInParent(); 
                if (processID < 0) {
                    continue;
                }
                used++;
                nanosleep(&ts, NULL); 

                long long start = nsecs();
                terminateBeforeWaitpid(processID);
                long long end = nsecs();

                time = end - start;
                realTime += time;
            }
            if (used == 0) { // used is 0 meaning we did not use the terminateBeforeWaitpid due to an error - no divide by 0 error
                fprintf(stderr, "Case 5: no successful forks\n"); 
                break; 
            }
            long long avgTime = (realTime / used) - avgEmpty4_8;
            double perCall = (double)avgTime;
            printf("Case 5: waitpid() when child already terminated\n");
            printf("AVG_EMPTY_SLOW %lld\n", avgEmpty4_8);
            printf("AVG_REAL_SLOW %lld\n", realTime / used);
            printf("PER_CALL_NS %.3f\n\n", perCall);
            break;
        }

        case 6: {
            long long realTime = 0;
            for (int i = 0; i < samplesSlow; i++) {
                long long start = nsecs();

                forkThenTerminateBeforeWaitpid();

                long long end = nsecs();

                time = end - start;
                realTime += time;
            }
            long long avgTime = (realTime / samplesSlow) - avgEmpty4_8;
            double perCall = (double)avgTime;
            printf("Case 6: fork, terminate child, then do waitpid, all within the timing this time\n");
            printf("AVG_EMPTY_SLOW %lld\n", avgEmpty4_8);
            printf("AVG_REAL_SLOW %lld\n", realTime / samplesSlow);
            printf("PER_CALL_NS %.3f\n\n", perCall);
            break;
        }

        case 7: {
            long long realTime = 0;
            for (int i = 0; i < samplesSlow; i++) {
                long long start = nsecs();

                runBinTrue();

                long long end = nsecs();

                time = end - start;
                realTime += time;
            }
            long long avgTime = (realTime / samplesSlow) - avgEmpty4_8;
            double perCall = (double)avgTime;
            printf("Case 7: perform system(/bin/true)\n");
            printf("AVG_EMPTY_SLOW %lld\n", avgEmpty4_8);
            printf("AVG_REAL_SLOW %lld\n", realTime / samplesSlow);
            printf("PER_CALL_NS %.3f\n\n", perCall);
            break;
        }

        case 8: {
            long long realTime = 0;
            for (int i = 0; i < samplesSlow; i++) {
                long long start = nsecs();

                createRemoveDir();

                long long end = nsecs();

                time = end - start;
                realTime += time;
            }
            long long avgTime = (realTime / samplesSlow) - avgEmpty4_8;
            double perCall = (double)avgTime;
            printf("Case 8: create then remove directory within /tmp on portal\n");
            printf("AVG_EMPTY_SLOW %lld\n", avgEmpty4_8);
            printf("AVG_REAL_SLOW %lld\n", realTime / samplesSlow);
            printf("PER_CALL_NS %.3f\n\n", perCall);
            break;
        }

        default: {
            fprintf(stderr, "option must be 1..8\n");
            return -1;
        }
    }

    return 0;
}
