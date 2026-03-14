#define _XOPEN_SOURCE 700
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "mlpt.h"

size_t ptbr = 0;


// Number of bits used to index into one level of the page table
// Each page table holds (page_size / 8) entries, so this equals POBITS - 3
static size_t index_bits(void) {
    return (size_t)POBITS - 3;
}


// Index into the page table at the given level for vpn
// Higher levels consume the most-significant portion of the VPN
static size_t vpn_index(size_t vpn, size_t lvl) {
    size_t bits = index_bits();
    size_t shift = bits * ((size_t)LEVELS - 1 - lvl);
    return (vpn >> shift) & (((size_t)1 << bits) - 1);
}


// Allocates and zeroes a page-aligned physical page via posix_memalign
// Returns NULL on failure
static void *alloc_page(void) {
    size_t page_size = (size_t)1 << POBITS;
    void *page = NULL;
    if (posix_memalign(&page, page_size, page_size) != 0) {
        return NULL;
    }
    memset(page, 0, page_size);
    return page;
}


// Builds a valid PTE from a physical page pointer
// uses (size_t)1 shift to avoid overflow when POBITS >= 32
static size_t make_pte(void *page) {
    return (((size_t)page) & ~(((size_t)1 << POBITS) - 1)) | 1;
}


// Extracts the physical base address encoded in a valid PTE
static size_t pte_base(size_t pte) {
    return pte & ~(((size_t)1 << POBITS) - 1);
}


// Ensures the intermediate page table at table[idx] exists - allocating
// one if the entry is not valid - returns pointer to next-level table, or null on failure
static size_t *ensure_next_table(size_t *table, size_t idx) {
    if ((table[idx] & 1) == 0) {
        void *next = alloc_page();
        if (next == NULL) {
            return NULL;
        }
        table[idx] = make_pte(next);
    }
    return (size_t *)pte_base(table[idx]);
}


// Translates virtual address va to a physical address using the multi-level page table rooted at ptbr
// Returns all-ones if the address is not mapped
size_t translate(size_t va) {
    if (ptbr == 0) {
        return ~(size_t)0;
    }
    size_t vpn = va >> POBITS;
    size_t offset = va & (((size_t)1 << POBITS) - 1);
    size_t *page_table = (size_t *)ptbr;

    for (size_t lvl = 0; lvl < (size_t)LEVELS; lvl += 1) {
        size_t pte = page_table[vpn_index(vpn, lvl)];
        if ((pte & 1) == 0) {
            return ~(size_t)0;
        }
        if (lvl == (size_t)LEVELS - 1) {
            return pte_base(pte) + offset;
        }
        page_table = (size_t *)pte_base(pte);
    }
    return ~(size_t)0;
}


// Allocates and maps the virtual page starting at start_va
// Returns -1 if start_va is not page-aligned, 0 if already mapped, or 1 if newly mapped
int allocate_page(size_t start_va) {
    if ((start_va & (((size_t)1 << POBITS) - 1)) != 0) {
        return -1;
    }
    if (ptbr == 0) {
        void *root = alloc_page();
        if (root == NULL) {
            return -1;
        }
        ptbr = (size_t)root;
    }

    size_t vpn = start_va >> POBITS;
    size_t *page_table = (size_t *)ptbr;

    for (size_t lvl = 0; lvl < (size_t)LEVELS - 1; lvl += 1) {
        page_table = ensure_next_table(page_table, vpn_index(vpn, lvl));
        if (page_table == NULL) {
            return -1;
        }
    }

    size_t last_idx = vpn_index(vpn, (size_t)LEVELS - 1);
    if ((page_table[last_idx] & 1) == 1) {
        return 0;
    }
    void *data = alloc_page();
    if (data == NULL) {
        return -1;
    }
    page_table[last_idx] = make_pte(data);
    return 1;
}


// Recursively frees all page tables and data pages reachable from table at given level - doesn't free table itself, that's the caller's job
static void free_table_recursive(size_t *table, size_t level) {
    size_t entries = ((size_t)1 << POBITS) / sizeof(size_t);
    for (size_t i = 0; i < entries; i += 1) {
        size_t pte = table[i];
        if ((pte & 1) == 0) {
            continue;
        }
        void *child = (void *)pte_base(pte);
        if (level + 1 < (size_t)LEVELS) {
            free_table_recursive((size_t *)child, level + 1);
        }
        free(child);
        table[i] = 0;
    }
}


// "destroys" tables by freeing all pages and resetting ptbr - calls thte recursive function above to do so
int mlpt_destroy(void) {
    if (ptbr == 0) {
        return -1;
    }
    free_table_recursive((size_t *)ptbr, 0);
    free((void *)ptbr);
    ptbr = 0;
    return 0;
}