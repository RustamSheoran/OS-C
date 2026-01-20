#include <stdint.h>
#include "kernel.h"
#include "pmm.h"

#define BITMAP_START 0x100000
#define PAGE_SIZE 4096

static uint8_t *bitmap;
static uint64_t bitmap_size;
static uint64_t total_pages;
static uint64_t free_pages;

void pmm_init(EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN MapSize, UINTN DescriptorSize) {
    // Calculate total conventional memory
    UINTN num_entries = MapSize / DescriptorSize;
    uint64_t max_addr = 0;
    for (UINTN i = 0; i < num_entries; i++) {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)MemoryMap + i * DescriptorSize);
        if (desc->Type == EfiConventionalMemory) {
            uint64_t end = desc->PhysicalStart + desc->NumberOfPages * PAGE_SIZE;
            if (end > max_addr) max_addr = end;
        }
    }
    total_pages = max_addr / PAGE_SIZE;
    bitmap_size = (total_pages + 7) / 8;
    bitmap = (uint8_t *)BITMAP_START;

    // Zero bitmap
    for (uint64_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0;
    }

    // Mark used pages
    for (UINTN i = 0; i < num_entries; i++) {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)MemoryMap + i * DescriptorSize);
        if (desc->Type != EfiConventionalMemory) {
            uint64_t start_page = desc->PhysicalStart / PAGE_SIZE;
            uint64_t end_page = start_page + desc->NumberOfPages;
            for (uint64_t p = start_page; p < end_page; p++) {
                bitmap[p / 8] |= (1 << (p % 8));
            }
        }
    }

    // Mark bitmap itself as used
    uint64_t bitmap_pages = (bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint64_t p = BITMAP_START / PAGE_SIZE; p < BITMAP_START / PAGE_SIZE + bitmap_pages; p++) {
        bitmap[p / 8] |= (1 << (p % 8));
    }

    // Count free pages
    free_pages = 0;
    for (uint64_t i = 0; i < total_pages; i++) {
        if ((bitmap[i / 8] & (1 << (i % 8))) == 0) free_pages++;
    }
}

uint64_t pmm_alloc_page() {
    for (uint64_t i = 0; i < total_pages; i++) {
        if ((bitmap[i / 8] & (1 << (i % 8))) == 0) {
            bitmap[i / 8] |= (1 << (i % 8));
            free_pages--;
            return i * PAGE_SIZE;
        }
    }
    return 0; // No free page
}

void pmm_free_page(uint64_t addr) {
    uint64_t page = addr / PAGE_SIZE;
    bitmap[page / 8] &= ~(1 << (page % 8));
    free_pages++;
}

uint64_t pmm_get_free_pages() {
    return free_pages;
}