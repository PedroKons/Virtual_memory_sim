#include "page_table.h"
#include <stdio.h>

PageTableEntry pageTable[PAGE_TABLE_SIZE];
char physical_memory[FRAME_COUNT][FRAME_SIZE];
int frame_usage[FRAME_COUNT];

void init_page_table() {
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        pageTable[i].valid = 0;
        pageTable[i].accessed = 0;
        pageTable[i].dirty = 0;
        pageTable[i].frameNumber = -1;
    }
    for (int i = 0; i < FRAME_COUNT; i++) {
        frame_usage[i] = 0;
    }
}

PageTableEntry* get_page_entry(int pageNumber) {
    if (pageNumber < 0 || pageNumber >= PAGE_TABLE_SIZE) return NULL;
    return &pageTable[pageNumber];
}

int load_page_from_backing_store(int pageNumber) {
    FILE* file = fopen("backing_store.txt", "rb");
    if (!file) {
        perror("Erro ao abrir backing_store.txt");
        return -1;
    }

    int frame = -1;
    for (int i = 0; i < FRAME_COUNT; i++) {
        if (!frame_usage[i]) {
            frame = i;
            break;
        }
    }

    if (frame == -1) {
        fclose(file);
        return -1;
    }

    fseek(file, pageNumber * FRAME_SIZE, SEEK_SET);
    fread(physical_memory[frame], sizeof(char), FRAME_SIZE, file);
    fclose(file);
    frame_usage[frame] = 1;

    return frame;
}
