#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include "mem.h"

#define PAGE_SIZE getpagesize()
#define ALIGNMENT 8

typedef struct block {
    size_t size;
    struct block *next;
} Block;

static Block *free_list = NULL;
static void *mem_region = NULL;
static int mem_size = 0;

static size_t align_size(size_t size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

int mem_init(int size_of_region) {
    if (size_of_region <= 0 || mem_region != NULL) {
        m_error = E_BAD_ARGS;
        return -1;
    }
    int num_pages = (size_of_region + PAGE_SIZE - 1) / PAGE_SIZE;
    mem_size = num_pages * PAGE_SIZE;
    mem_region = mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem_region == MAP_FAILED) {
        m_error = E_BAD_ARGS;
        return -1;
    }
    free_list = (Block *)mem_region;
    free_list->size = mem_size - sizeof(Block);
    free_list->next = NULL;

    return 0;
}

void *mem_alloc(int size, int style) {
    size = align_size(size);
    void *allocated_mem = NULL;
    Block *current_block = free_list;
    Block *prev_block = NULL;
    Block *split_block = NULL;

    // Check if there is enough free space
    while (current_block != NULL) {
        if (current_block->size >= size) {
            // Split the block if there's enough space for a new block
            if (current_block->size >= size + sizeof(Block)) {
                split_block = (Block *)((char *)current_block + sizeof(Block) + size);
                split_block->size = current_block->size - sizeof(Block) - size;
                split_block->next = current_block->next;
                current_block->next = split_block;
            }
            allocated_mem = (void *)((char *)current_block + sizeof(Block));
            current_block->size = size;

            // Remove the block from the free list if it's fully allocated
            if (split_block != NULL || current_block == free_list) {
                free_list = split_block != NULL ? split_block : current_block->next;
            } else {
                prev_block->next = current_block->next;
            }
            return allocated_mem;
        }
        prev_block = current_block;
        current_block = current_block->next;
    }
    m_error = E_NO_SPACE;
    return NULL;
}

int mem_free(void *ptr) {
    if (ptr == NULL) {
        return -1;
    }
    Block *block_to_free = (Block *)((char *)ptr - sizeof(Block));
    Block *current_block = free_list;
    Block *prev_block = NULL;

    while (current_block != NULL && current_block < block_to_free) {
        prev_block = current_block;
        current_block = current_block->next;
    }

    // Merge with the previous block if adjacent
    if (prev_block != NULL && (char *)prev_block + sizeof(Block) + prev_block->size == (char *)block_to_free) {
        prev_block->size += sizeof(Block) + block_to_free->size;
        block_to_free = prev_block;
    } else {
        block_to_free->next = current_block;
        prev_block = block_to_free;
    }

    // Merge with the next block if adjacent
    if (current_block != NULL && (char *)block_to_free + sizeof(Block) + block_to_free->size == (char *)current_block) {
        block_to_free->size += sizeof(Block) + current_block->size;
        block_to_free->next = current_block->next;
        if (prev_block != NULL)
            prev_block->next = block_to_free;
    } else {
        prev_block->next = block_to_free;
    }

    // Update the free list if the freed block is at the beginning
    if (prev_block == NULL) {
        free_list = block_to_free;
    }

    return 0;
}

void mem_dump() {
    printf("Memory Dump:\n");
    printf("-------------\n");
    Block *current_block = free_list;
    while (current_block != NULL) {
        printf("Free Block: %p, Size: %zu\n", current_block, current_block->size);
        current_block = current_block->next;
    }
    printf("-------------\n");
}

int main() {
    if (mem_init(1024) == -1) {
        printf("Memory initialization failed.\n");
        return 1;
    }

    // Allocate memory using mem_alloc
    void *ptr = mem_alloc(32, M_FIRSTFIT);
    if (ptr == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    mem_dump();
    return 0;
}
