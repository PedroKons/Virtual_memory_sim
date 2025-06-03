#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#define PAGE_TABLE_SIZE 32
#define FRAME_COUNT 32
#define FRAME_SIZE 256

typedef struct {
    int valid;
    int accessed;
    int dirty;
    int frameNumber;
} PageTableEntry;

extern char physical_memory[FRAME_COUNT][FRAME_SIZE];
extern int frame_usage[FRAME_COUNT];

void init_page_table();
PageTableEntry* get_page_entry(int pageNumber);
int load_page_from_backing_store(int pageNumber);

#endif
