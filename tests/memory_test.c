#include "pmm.h"
#include "paging.h"

extern void *kmalloc(size_t size);

int test_pfa_stress() {
    uint64_t pages[100];
    for (int i = 0; i < 100; i++) {
        pages[i] = pmm_alloc_page();
        if (!pages[i]) return 0;
    }
    for (int i = 0; i < 100; i++) {
        pmm_free_page(pages[i]);
    }
    return 1;
}

int test_heap_stress() {
    void *ptrs[50];
    for (int i = 0; i < 50; i++) {
        ptrs[i] = kmalloc(100);
        if (!ptrs[i]) return 0;
    }
    // No free for bump
    return 1;
}

int test_paging_stress() {
    // Map many pages
    for (uint64_t addr = 0x100000000; addr < 0x100000000 + 0x100000; addr += 0x1000) {
        if (!map_page(addr, pmm_alloc_page(), 3)) return 0; // Simplified
    }
    return 1;
}