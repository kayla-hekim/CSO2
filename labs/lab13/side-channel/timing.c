#include "timing.h"

#include <stdint.h>
#include <stdlib.h>


/*
 * See https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf
 */

long measure_once(int *presult, char *p, int (*f)(char *)) {
    uint32_t start_cycles_high, start_cycles_low;
    uint32_t end_cycles_high, end_cycles_low;
    __asm__ volatile(
        "cpuid\n\t"
        "rdtsc\n\t"
        :
        "=d" (start_cycles_high), "=a" (start_cycles_low)
        :: "%rbx", "%rcx"
    );
    *presult = f(p);
    __asm__ volatile(
        "rdtscp\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "cpuid\n\t"
        :
        "=r" (end_cycles_high), "=r" (end_cycles_low)
        :: "%rax", "%rbx", "%rcx", "%rdx"
    );
    uint64_t start_cycles = ((uint64_t) start_cycles_high) << 32 | start_cycles_low;
    uint64_t end_cycles = ((uint64_t) end_cycles_high) << 32 | end_cycles_low;
    return end_cycles - start_cycles;
}

