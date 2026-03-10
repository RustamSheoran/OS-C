#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "pmm.h"

#define BITMAP_START 0x00100000ULL
#define PAGE_SIZE 4096ULL

static uint8_t *bitmap;
static uint64_t bitmap_size;
static uint64_t total_pages;
static uint64_t free_pages;

static inline void mark_used(uint64_t page) {
    bitmap[page / 8ULL] |= (uint8_t)(1U << (page % 8ULL));
}

static inline void mark_free(uint64_t page) {
    bitmap[page / 8ULL] &= (uint8_t)~(1U << (page % 8ULL));
}

static inline int is_used(uint64_t page) {
    return (bitmap[page / 8ULL] & (uint8_t)(1U << (page % 8ULL))) != 0;
}

void pmm_init(EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN MapSize, UINTN DescriptorSize) {
    UINTN num_entries = MapSize / DescriptorSize;
    uint64_t max_addr = 0;

    for (UINTN i = 0; i < num_entries; i++) {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)MemoryMap + i * DescriptorSize);
        uint64_t end = desc->PhysicalStart + desc->NumberOfPages * PAGE_SIZE;
        if (end > max_addr) {
            max_addr = end;
        }
    }

    total_pages = max_addr / PAGE_SIZE;
    bitmap_size = (total_pages + 7ULL) / 8ULL;
    bitmap = (uint8_t *)(uintptr_t)BITMAP_START;

    for (uint64_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0xFF;
    }

    for (UINTN i = 0; i < num_entries; i++) {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)MemoryMap + i * DescriptorSize);
        if (desc->Type == EfiConventionalMemory) {
            uint64_t start_page = desc->PhysicalStart / PAGE_SIZE;
            uint64_t end_page = start_page + desc->NumberOfPages;
            if (end_page > total_pages) {
                end_page = total_pages;
            }
            for (uint64_t p = start_page; p < end_page; p++) {
                mark_free(p);
            }
        }
    }

    uint64_t bitmap_pages = (bitmap_size + PAGE_SIZE - 1ULL) / PAGE_SIZE;
    uint64_t bitmap_start_page = BITMAP_START / PAGE_SIZE;
    for (uint64_t p = bitmap_start_page; p < bitmap_start_page + bitmap_pages && p < total_pages; p++) {
        mark_used(p);
    }

    if (total_pages > 0) {
        mark_used(0);
    }

    free_pages = 0;
    for (uint64_t i = 0; i < total_pages; i++) {
        if (!is_used(i)) {
            free_pages++;
        }
    }
}

uint64_t pmm_alloc_page() {
    if (bitmap == NULL || total_pages == 0 || free_pages == 0) {
        return 0;
    }

    for (uint64_t i = 1; i < total_pages; i++) {
        if (!is_used(i)) {
            mark_used(i);
            if (free_pages > 0) {
                free_pages--;
            }
            return i * PAGE_SIZE;
        }
    }
    return 0;
}

void pmm_free_page(uint64_t addr) {
    if (addr == 0) {
        return;
    }

    uint64_t page = addr / PAGE_SIZE;
    if (page >= total_pages || bitmap == NULL) {
        return;
    }
    if (is_used(page)) {
        mark_free(page);
        free_pages++;
    }
}

uint64_t pmm_get_free_pages() {
    return free_pages;
}
