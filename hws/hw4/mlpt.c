#define _XOPEN_SOURCE 700
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "mlpt.h"

size_t ptbr = 0;


// MULTI TABLE TRANSLATE
size_t translate(size_t va) {
    if (ptbr == 0) {
        return ~(size_t)0;
    }

    // size of the pages
    size_t page_size = (size_t)1 << POBITS;

    // get vpn and offset bits
    size_t vpn = va >> POBITS;
    size_t offset = va & (page_size - 1);

    size_t index_bits = (size_t)POBITS - 3;
    size_t index_mask = ((size_t)1 << index_bits) - 1;

    // page_table at the base register, then go to the indexed entry in page_table based on vpn
    size_t *page_table = (size_t *)ptbr;

    // LOOP OVER THE VPN IN THE VA:
    for (size_t lvl = 0; lvl < (size_t)LEVELS; lvl += 1) {
        size_t shift = index_bits * ((size_t)LEVELS - 1 - lvl);
        size_t idx = (vpn >> shift) & index_mask;

        size_t pte = page_table[idx];
        if ((pte & 1) == 0) {
            return ~(size_t)0;
        }

        size_t next_base = pte & ~(page_size - 1);

        if (lvl == (size_t)LEVELS - 1) {
            return next_base + offset;
        }

        page_table = (size_t *)next_base;
    }

    // should never hit here if valid
    return ~(size_t)0;
}


// MULTI TABLE ALLOCATE_PAGE
int allocate_page(size_t start_va) {
    size_t page_size = (size_t)1 << POBITS;
    size_t offset_mask = page_size - 1;
    size_t offset = start_va & offset_mask;
    if (offset != 0) {
        return -1;
    }

    // allocate page if ptbr is 0 or hasn't been allocated yet
    if (ptbr == 0) {
        void *root = NULL;
        int rc = posix_memalign(&root, page_size, page_size);
        if (rc != 0) {
            return -1;
        }
        memset(root, 0, page_size);
        ptbr = (size_t)root;
    }

    // calculate vpn by shifting virtual address over POBITS size
    size_t vpn = start_va >> POBITS;

    size_t index_bits = (size_t)POBITS - 3;
    size_t index_mask = ((size_t)1 << index_bits) - 1;
    
    size_t *page_table = (size_t *)ptbr;
    
    // loop over levels in page table that need to be included and traverse to find final data
    for (size_t lvl = 0; lvl < (size_t)LEVELS; lvl += 1) {
        size_t shift = index_bits * ((size_t)LEVELS - 1 - lvl);
        size_t idx = (vpn >> shift) & index_mask;

        size_t pte = page_table[idx];

        // allocating new physical page - gets ppn and new page table entry
        if (lvl == (size_t)LEVELS - 1) {
            // Last level: map to data page
            if ((pte & 1) == 1) {
                return 0; // already allocated
            }

            void *data_page = NULL;
            int rc2 = posix_memalign(&data_page, page_size, page_size);
            if (rc2 != 0) {
                return -1;
            }
            memset(data_page, 0, page_size);
            size_t ppn = ((size_t)data_page) >> POBITS;
            page_table[idx] = (ppn << POBITS) | 1;

            return 1; // newly allocated per instructions guideline
        }

        // Intermediate level: ensure next-level page table exists
        if ((pte & 1) == 0) {
            void *next_table_page = NULL;
            int rc3 = posix_memalign(&next_table_page, page_size, page_size);
            if (rc3 != 0) {
                return -1;
            }
            memset(next_table_page, 0, page_size);

            size_t next_ppn = ((size_t)next_table_page) >> POBITS;
            page_table[idx] = (next_ppn << POBITS) | 1;

            pte = page_table[idx];
        }

        size_t next_base = pte & ~(page_size - 1);
        page_table = (size_t *)next_base;
    }

    return -1; // page was unable to be allocated/error
}


static void free_table_recursive(size_t *table, size_t level) {
    size_t page_size = (size_t)1 << POBITS;
    size_t entries = page_size / sizeof(size_t);

    for (size_t i = 0; i < entries; i += 1) {
        size_t pte = table[i];
        if ((pte & 1) == 0) {
            continue;
        }

        size_t child_base = pte & ~(page_size - 1);
        void *child_ptr = (void *)child_base;

        if (level + 1 < (size_t)LEVELS) {
            // child is another page table
            free_table_recursive((size_t *)child_ptr, level + 1);
            free(child_ptr);
        }
        else {
            // child is a data page
            free(child_ptr);
        }

        table[i] = 0; // optional: clear entry after freeing
    }
}


int mlpt_destroy(void) {
    if (ptbr == 0) {
        return -1;
    }

    void *root_ptr = (void *)ptbr;

    free_table_recursive((size_t *)root_ptr, 0);

    free(root_ptr);
    ptbr = 0;
    return 0;
}

