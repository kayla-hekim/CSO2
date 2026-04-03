#include <stdio.h>
#include <stdalign.h>
#include <assert.h>
#include "mlpt.h"
#include "config.h"


int main(void) {
    //PART 1: test allocate:
    assert(ptbr == 0);

    int r = allocate_page(0x3000);
    assert(r == 1 || r == 0);
    assert(ptbr != 0);

    size_t *pointer_to_table;
    pointer_to_table = (size_t *) ptbr;
    size_t page_table_entry = pointer_to_table[3];
    printf("PTE @ index 3: valid bit=%d  physical page number=0x%lx\n",
        (int) (page_table_entry & 1),
        (long) (page_table_entry >> 12)
    );
}