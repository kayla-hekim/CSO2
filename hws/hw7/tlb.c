#include <stddef.h>
#include "config.h" /* see pagtable guidance on this file */
#include "mlpt.h"   /* see pagetable this file */
#include "tlb.h"

typedef struct { // valid bit | VPN bits | physical address base number | least recently used  -->  tlb entry
    int valid;
    size_t vpn;
    size_t pa_base;
    int lru;
} tlb_entry_t;

static tlb_entry_t tlb[16][4]; // tlb = 16 rows with 4 ways each
// tlb is 16 rows -> least significant 4 bits = index bits (4); rest are tag = vpn bits - 4


/** invalidate all cache lines in the TLB */
void tlb_clear() {
    for (int i = 0; i < 16; i+=1) {
        for (int j = 0; j < 4; j+=1) {
            tlb_entry_t *way = &tlb[i][j];

            way->valid = 0;
            way->vpn = 0;
            way->pa_base = 0;
            way->lru = 0;
        }
    }
}

/**
 * return 0 if this virtual address does not have a valid
 * mapping in the TLB. Otherwise, return its LRU status: 1
 * if it is the most-recently used, 2 if the next-to-most,
 * etc.
 */
int tlb_peek(size_t va) {
    size_t vpn = va >> POBITS;
    size_t index = vpn & 0x0F;

    tlb_entry_t *row = tlb[index];

    for (int i = 0; i < 4; i+=1) {
        if (row[i].vpn == vpn && row[i].valid == 1) {
            return row[i].lru;
        }
    }

    return 0;
}

/**
 * If this virtual address is in the TLB, return its
 * corresponding physical address. If not, use
 * `translate(va)` to find that address, store the result
 * in the TLB, and return it. In either case, make its
 * cache line the most-recently used in its set.
 *
 * As an exception, if translate(va) returns -1, do not
 * update the TLB: just return -1.
 */
size_t tlb_translate(size_t va) {
    size_t vpn = va >> POBITS;
    size_t index = vpn & 0x0F;
    size_t offset = va & (((size_t)1 << POBITS) - 1);
    size_t page_va = va & ~(((size_t)1 << POBITS) - 1);

    tlb_entry_t *row = tlb[index];

    for (int i = 0; i < 4; i+=1) {
        if (row[i].vpn == vpn && row[i].valid == 1) {
            // HIT LOGIC
            size_t pa = row[i].pa_base + offset;
            
            int old_lru = row[i].lru; // old lru to now update others from before it - putting new lru at beginning
            for (int j = 0; j < 4; j += 1) {
                if (row[j].valid && row[j].lru < old_lru) {
                    row[j].lru += 1;
                }
            }
            row[i].lru = 1; // putting a beginning (most recently used)

            return pa;
        }
    }

    // WERE UNABLE TO FIND VALID VPN IN ROW - MISS
    size_t ret_translate_pa = translate(page_va);
    if (ret_translate_pa == ~(size_t)0) {
        return ~(size_t)0;
    }

    // the way isn't marked valid but this is where we'll insert the data into way
    int target = -1;
    for (int i = 0; i < 4; i += 1) {
        if (!row[i].valid) {
            target = i;
            break;
        }
    }

    // we missed -> find lru with 4 (LRU) then that is the target to replace on this miss
    if (target == -1) {
        for (int i = 0; i < 4; i += 1) {
            if (row[i].lru == 4) {
                target = i;
                break;
            }
        }
    }
    // bump all lru bits in ways in set up 1 - insert last one at the bginning of 1 (MRU)
    for (int i = 0; i < 4; i += 1) {
        if (row[i].valid && i != target && row[i].lru < 4) {
            row[i].lru += 1;
        }
    }

    // insert new stuff into tlb table
    row[target].valid = 1;
    row[target].vpn = vpn;
    row[target].pa_base = ret_translate_pa;
    row[target].lru = 1;

    //other stuff

    return ret_translate_pa + offset;
}