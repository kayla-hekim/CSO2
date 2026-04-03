#include <stdio.h>
#include <stdalign.h>
#include <assert.h>
#include "mlpt.h"
#include "config.h"

alignas(4096)
static size_t testing_page_table[512];
alignas(4096)
static char data_page[4096];


static void setup_ptbr(void) {
    ptbr = (size_t)&testing_page_table[0];

    size_t addr = (size_t)&data_page[0];
    size_t ppn = addr >> POBITS;

    size_t pte = (ppn << POBITS) | 1;  // valid bit

    testing_page_table[3] = pte;       // map VPN 3
}


int main(void) {
    //PART 1: test translate:
    setup_ptbr();

    size_t result = translate(0x3045);

    printf("translated = %p\n", (void*)result);
    printf("expected   = %p\n", &data_page[0x45]);

    assert(result == (size_t)&data_page[0x45]);

    printf("translate() works!\n");
    return 0;

}