#define _GNU_SOURCE
#include "util.h"
#include <stdio.h>      // for printf
#include <stdlib.h>     // for atoi (and malloc() which you'll likely use)
#include <sys/mman.h>   // for mmap() which you'll likely use
#include <stdalign.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>



alignas(4096) volatile char global_array[4096 * 32];


static uintptr_t get_heap_end(void) {
    FILE *f = fopen("/proc/self/maps", "r");
    if (!f) { perror("maps"); exit(1); }

    char line[512];
    while (fgets(line, sizeof line, f)) {
        if (strstr(line, "[heap]")) {
            unsigned long start, end;
            sscanf(line, "%lx-%lx", &start, &end);
            fclose(f);
            return (uintptr_t)end;
        }
    }

    fprintf(stderr, "heap not found\n");
    exit(1);
}



void labStuff(int which) {
    if (which == 0) {
        /* do nothing */
        
    } else if (which == 1) {
        for (int i = 0; i < 32; i++) { // populate the global array global_array with 32 pages worth of bytes - 1 byte per page
            global_array[i * 4096] = (char)i;
        }
        for (int j = 0; j < 100000; j++) { // touch first page of array multiple times (1 per iteration)
            global_array[0] = 7;
        }

    } else if (which == 2) {
        size_t size = 1048576; // 1024 * 1024
        volatile char *pointer = malloc(size); // volatile helps with changes outside control - will change
        if (!pointer) { 
            perror("malloc"); exit(1); 
        }
        pointer[0] = 7; //1st element
        pointer[500] = 7; // 500th element
        pointer[size - 1] = 8; // last element

    } else if (which == 3) {
        size_t size = 1048576;
        // mmap to avoid malloc weirdness
        volatile char *pointer = 
            mmap(NULL,
                size,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS,
                -1,
                0);
        if (pointer == MAP_FAILED) { // handle failure
            perror("mmap caused the failure in 3");
            exit(1);
        }
        for (int page = 0; page < 32; page++) { // 131072 / 4096 = 32 pages -> touch 32 pages to touch 131072 bytes
            pointer[page * 4096] = 7; 
        }

    } else if (which == 4) {
        uintptr_t heap_end = get_heap_end();
        uintptr_t target = (heap_end + 0x200000 + 0xFFF) & ~(uintptr_t)0xFFF;
        
        volatile char *pointer = mmap((void*) target,
                4096,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
                -1,
                0
        );
        if (pointer == MAP_FAILED) { 
            perror("mmap which==4");
            exit(1); 
        }
        for (int i = 0; i < 4096; i++) { // touch every byte within this newly allocated page
            pointer[i] = (char)i; 
        }

    } else if (which == 5) {
        uintptr_t heap_end = get_heap_end();
        uintptr_t target = (heap_end + 0x10000000000ULL + 0xFFF) & ~(uintptr_t)0xFFF;

        volatile char *pointer = mmap((void*) target,
                4096,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
                -1,
                0
        );
        if (pointer == MAP_FAILED) { 
            perror("mmap which==5");
            exit(1); 
        }
        for (int i = 0; i < 4096; i++) { // touch every byte within this newly allocated page
            pointer[i] = (char)i; 
        }
    }
}



int main(int argc, char **argv) {
    int which = 0;
    if (argc > 1) {
        which = atoi(argv[1]);
    } else {
        fprintf(stderr, "Usage: %s NUMBER\n", argv[0]);
        return 1;
    }
    printf("Memory layout:\n");
    print_maps(stdout);
    printf("\n");
    printf("Initial state:\n");
    force_load();
    struct memory_record r1, r2;
    record_memory_record(&r1);
    print_memory_record(stdout, NULL, &r1);
    printf("---\n");

    printf("Running labStuff(%d)...\n", which);

    labStuff(which);

    printf("---\n");
    printf("Afterwards:\n");
    record_memory_record(&r2);
    print_memory_record(stdout, &r1, &r2);
    print_maps(stdout);
    return 0;
}
