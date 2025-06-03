#include "tlb.h"

static TLBEntry tlb[TLB_SIZE];
static unsigned long time_counter = 0;

void init_tlb() {
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].valid = 0;
    }
}

int search_tlb(int pageNumber, int* frameNumber) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].pageNumber == pageNumber) {
            *frameNumber = tlb[i].frameNumber;
            tlb[i].lastUsed = ++time_counter;
            return 1;
        }
    }
    return 0;
}

void update_tlb(int pageNumber, int frameNumber) {
    int lruIndex = 0;
    unsigned long oldest = tlb[0].lastUsed;

    for (int i = 1; i < TLB_SIZE; i++) {
        if (!tlb[i].valid) {
            lruIndex = i;
            break;
        }
        if (tlb[i].lastUsed < oldest) {
            oldest = tlb[i].lastUsed;
            lruIndex = i;
        }
    }

    tlb[lruIndex].valid = 1;
    tlb[lruIndex].pageNumber = pageNumber;
    tlb[lruIndex].frameNumber = frameNumber;
    tlb[lruIndex].lastUsed = ++time_counter;
}
