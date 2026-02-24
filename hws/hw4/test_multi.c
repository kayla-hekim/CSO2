#include <stdio.h>
#include <stdalign.h>
#include <assert.h>
#include "mlpt.h"
#include "config.h"


int main(void) {
    //PART 2: test multi layer table:
    // 0 pages have been allocated
    assert(ptbr == 0);

    allocate_page(0x456789abc000);
    // 5 pages have been allocated: 4 page tables and 1 data
    assert(ptbr != 0);

    allocate_page(0x456789abc000);
    // no new pages allocated (still 5)

    int *p1 = (int *)translate(0x456789abcd00);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

    allocate_page(0x456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)

    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

    allocate_page(0x456780000000);
    // 2 new pages allocated (now 8; 5 page table, 3 data)
}