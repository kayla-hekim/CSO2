// #include <stdio.h>
// #include <stdalign.h>
// #include <assert.h>
// #include "mlpt.h"
// #include "config.h"


// int main(void) {
//     //PART 2: test multi layer table:
//     // 0 pages have been allocated
//     assert(ptbr == 0);

//     allocate_page(0x456789abc000);
//     // 5 pages have been allocated: 4 page tables and 1 data
//     assert(ptbr != 0);

//     allocate_page(0x456789abc000);
//     // no new pages allocated (still 5)

//     int *p1 = (int *)translate(0x456789abcd00);
//     *p1 = 0xaabbccdd;
//     short *p2 = (short *)translate(0x456789abcd02);
//     printf("%04hx\n", *p2); // prints "aabb\n"

//     assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

//     allocate_page(0x456789ab0000);
//     // 1 new page allocated (now 6; 4 page table, 2 data)

//     assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

//     allocate_page(0x456780000000);
//     // 2 new pages allocated (now 8; 5 page table, 3 data)
// }

////////////////////////////////////////////

// #include <stdio.h>
// #include <stdalign.h>
// #include <assert.h>
// #include "mlpt.h"
// #include "config.h"

// alignas(4096) static size_t root_table[512];
// alignas(4096) static size_t second_table[512];
// alignas(4096) static char data_page[4096];

// static size_t make_pte(void *page) {
//     size_t ppn = ((size_t)page) >> POBITS;
//     return (ppn << POBITS) | 1;
// }

// static void setup_ptbr_2level(void) {
//     // clear tables (global statics are already 0, but explicit is fine)
//     for (int i = 0; i < 512; i += 1) {
//         root_table[i] = 0;
//         second_table[i] = 0;
//     }

//     ptbr = (size_t)&root_table[0];

//     // map VPN=3 for va 0x3045 (vpn=3, idx0=0, idx1=3 when LEVELS=2, POBITS=12)
//     root_table[0] = make_pte(&second_table[0]);
//     second_table[3] = make_pte(&data_page[0]);
// }

// int main(void) {
//     setup_ptbr_2level();

//     size_t result = translate(0x3045);

//     printf("translated = %p\n", (void*)result);
//     printf("expected   = %p\n", &data_page[0x45]);

//     assert(result == (size_t)&data_page[0x45]);

//     printf("translate() works for LEVELS=2!\n");
//     return 0;
// }

////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "mlpt.h"
#include "config.h"

static int is_all_ones(size_t x) {
    return x == ~(size_t)0;
}

int main(void) {
    // 1) ptbr starts NULL
    assert(ptbr == 0);

    // Choose a couple page-aligned VAs that differ in VPN bits
    size_t va1 = 0x456789abc000;
    size_t va2 = 0x456789ab0000;   // different VPN path in the handout examples
    size_t va3 = 0x456780000000;   // another different path

    // 2) Before allocation, translate should fail
    assert(is_all_ones(translate(va1)));
    assert(is_all_ones(translate(va2)));

    // 3) First allocate: should create tables + data page
    int r1 = allocate_page(va1);
    assert(r1 == 1);
    assert(ptbr != 0);
    assert(!is_all_ones(translate(va1)));

    // 4) Second allocate same page: should return 0 (no changes)
    int r1b = allocate_page(va1);
    assert(r1b == 0);

    // 5) Writes/reads through translate must be consistent
    int *p = (int *)translate(va1 + 0x100);
    assert(!is_all_ones((size_t)p));
    *p = 0x11223344;

    short *q = (short *)translate(va1 + 0x102);
    assert(!is_all_ones((size_t)q));
    printf("read back short = %04hx\n", *q);

    // 6) An unmapped page should still fail
    assert(is_all_ones(translate(va2)));

    // 7) Allocate a second page: returns 1, translate works
    int r2 = allocate_page(va2);
    assert(r2 == 1);
    assert(!is_all_ones(translate(va2)));

    // 8) Ensure va1 and va2 are not mapped to the same physical page base
    // Compare translated addresses with offset 0 (page base)
    size_t pa1 = translate(va1) & ~(((size_t)1 << POBITS) - 1);
    size_t pa2 = translate(va2) & ~(((size_t)1 << POBITS) - 1);
    assert(pa1 != pa2);

    // 9) Third allocation (another path)
    int r3 = allocate_page(va3);
    assert(r3 == 1);
    assert(!is_all_ones(translate(va3)));

    puts("allocate_page multi-level test passed.");
    return 0;
}