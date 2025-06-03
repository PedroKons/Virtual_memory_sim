#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include "page_table.h"
#include "tlb.h"

#define PAGE_SIZE 256
#define MAX_THREADS 8

typedef struct {
    uint16_t address;
    int id;
} ThreadArg;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* process_address(void* arg) {
    ThreadArg* data = (ThreadArg*)arg;
    uint16_t address = data->address; 

    int page_number = address / PAGE_SIZE;
    int offset = address % PAGE_SIZE;
    int frame = -1;

    pthread_mutex_lock(&mutex);
    printf("[Thread %d] Endereço virtual: %u → Página: %d | Offset: %d\n",
           data->id, address, page_number, offset);
    pthread_mutex_unlock(&mutex);

    if (page_number < 0 || page_number >= PAGE_TABLE_SIZE) {
        pthread_mutex_lock(&mutex);
        printf("[Thread %d] ❌ Página fora do intervalo: %d (máximo permitido: %d)\n",
               data->id, page_number, PAGE_TABLE_SIZE - 1);
        pthread_mutex_unlock(&mutex);
        free(data);
        pthread_exit(NULL);
    }

    if (search_tlb(page_number, &frame)) {
        pthread_mutex_lock(&mutex);
        printf("[Thread %d] ✅ TLB Hit → Frame: %d\n", data->id, frame);
        pthread_mutex_unlock(&mutex);
    } else {
        PageTableEntry* entry = get_page_entry(page_number);

        if (!entry->valid) {
            pthread_mutex_lock(&mutex);
            printf("[Thread %d] ⚠️ Page Fault: Página %d não está na RAM.\n", data->id, page_number);
            pthread_mutex_unlock(&mutex);

            int frameFromDisk = load_page_from_backing_store(page_number);
            if (frameFromDisk == -1) {
                pthread_mutex_lock(&mutex);
                printf("[Thread %d] ❌ Falha ao carregar página %d da backing store.\n", data->id, page_number);
                pthread_mutex_unlock(&mutex);
                free(data);
                pthread_exit(NULL);
            }

            entry->valid = 1;
            entry->accessed = 1;
            entry->dirty = 0;
            entry->frameNumber = frameFromDisk;

            pthread_mutex_lock(&mutex);
            printf("[Thread %d] ✅ Página %d carregada no frame %d.\n",
                   data->id, page_number, frameFromDisk);
            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_lock(&mutex);
            printf("[Thread %d] ✅ Page Hit: Página %d já está no frame %d.\n",
                   data->id, page_number, entry->frameNumber);
            pthread_mutex_unlock(&mutex);
        }

        update_tlb(page_number, entry->frameNumber);
        frame = entry->frameNumber;
    }

    FILE* data_mem = fopen("data_memory.txt", "rb");
    if (data_mem) {
        fseek(data_mem, frame * PAGE_SIZE + offset, SEEK_SET);
        int value = fgetc(data_mem);
        fclose(data_mem);

        pthread_mutex_lock(&mutex);
        printf("[Thread %d] 📥 Valor lido da memória: %d\n", data->id, value);
        pthread_mutex_unlock(&mutex);
    } else {
        pthread_mutex_lock(&mutex);
        printf("[Thread %d] ❌ ERRO: Não foi possível abrir data_memory.txt\n", data->id);
        pthread_mutex_unlock(&mutex);
    }

    free(data);
    pthread_exit(NULL);
}

int main() {
    FILE* file = fopen("addresses_16b.txt", "r");
    if (!file) {
        perror("Erro ao abrir addresses_16b.txt");
        return 1;
    }

    init_page_table();
    init_tlb();

    pthread_t threads[MAX_THREADS];
    char line[64];
    int thread_count = 0;

    while (fgets(line, sizeof(line), file)) {
        uint32_t address;
        if (line[0] == '0' && line[1] == 'x') {
            address = strtol(line, NULL, 16);
        } else {
            address = atoi(line);
        }

        if (address >= (1 << 16)) {
            printf("Endereço inválido (> 16 bits): %u\n", address);
            continue;
        }

        printf("Endereço lido do arquivo: %u (0x%x)\n", address, address);

        ThreadArg* arg = malloc(sizeof(ThreadArg));
        arg->address = address;
        arg->id = thread_count;

        pthread_create(&threads[thread_count], NULL, process_address, arg);
        thread_count++;

        if (thread_count >= MAX_THREADS) {
            for (int i = 0; i < thread_count; i++) {
                pthread_join(threads[i], NULL);
            }
            thread_count = 0;
        }
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(file);
    return 0;
}
