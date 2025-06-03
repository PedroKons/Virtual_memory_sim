#ifndef TLB_H
#define TLB_H

#define TLB_SIZE 16

typedef struct {
    int pageNumber;
    int frameNumber;
    int valid;
    unsigned long lastUsed;
} TLBEntry;

void init_tlb();
int search_tlb(int pageNumber, int* frameNumber);
void update_tlb(int pageNumber, int frameNumber);

#endif
