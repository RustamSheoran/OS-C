#include <stdint.h>
#include "pmm.h"
#include "paging.h"
#include "io.h"

#define PAGE_SIZE 4096
#define PRESENT (1ULL << 0)
#define WRITE (1ULL << 1)
#define USER (1ULL << 2)

static uint64_t *pml4;

void map_page(uint64_t virt, uint64_t phys, uint64_t flags);

void paging_init(uint64_t kernel_base) {
    pml4 = (uint64_t *)pmm_alloc_page();
    if (!pml4) return;

    // Zero PML4
    for (int i = 0; i < 512; i++) pml4[i] = 0;

    // Map low 2MB identity
    for (uint64_t addr = 0; addr < 0x200000; addr += PAGE_SIZE) {
        map_page(addr, addr, PRESENT | WRITE);
    }

    // Map kernel higher-half to physical kernel_base
    uint64_t kernel_virt = 0xFFFFFFFF80000000;
    uint64_t kernel_size = 0x200000; // Assume 2MB
    for (uint64_t offset = 0; offset < kernel_size; offset += PAGE_SIZE) {
        map_page(kernel_virt + offset, kernel_base + offset, PRESENT | WRITE);
    }

    // Set CR3
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pml4) : "memory");

    // Map LAPIC
    uint64_t lapic_phys = rdmsr(0x1B) & ~0xFFFULL;
    for (uint64_t offset = 0; offset < 0x1000; offset += PAGE_SIZE) {
        map_page(lapic_phys + offset, 0xFFFFFFFFFF000000ULL + offset, PRESENT | WRITE);
    }

    // Map IOAPIC
    for (uint64_t offset = 0; offset < 0x1000; offset += PAGE_SIZE) {
        map_page(0xFEC00000 + offset, 0xFFFFFFFFFF001000ULL + offset, PRESENT | WRITE);
    }
}

void map_page(uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t pml4_index = (virt >> 39) & 0x1FF;
    uint64_t pdp_index = (virt >> 30) & 0x1FF;
    uint64_t pd_index = (virt >> 21) & 0x1FF;
    uint64_t pt_index = (virt >> 12) & 0x1FF;

    uint64_t *pdp;
    if (!(pml4[pml4_index] & PRESENT)) {
        pdp = (uint64_t *)pmm_alloc_page();
        if (!pdp) return;
        for (int i = 0; i < 512; i++) pdp[i] = 0;
        pml4[pml4_index] = ((uint64_t)pdp) | PRESENT | WRITE;
    } else {
        pdp = (uint64_t *)(pml4[pml4_index] & ~0xFFFULL);
    }

    uint64_t *pd;
    if (!(pdp[pdp_index] & PRESENT)) {
        pd = (uint64_t *)pmm_alloc_page();
        if (!pd) return;
        for (int i = 0; i < 512; i++) pd[i] = 0;
        pdp[pdp_index] = ((uint64_t)pd) | PRESENT | WRITE;
    } else {
        pd = (uint64_t *)(pdp[pdp_index] & ~0xFFFULL);
    }

    uint64_t *pt;
    if (!(pd[pd_index] & PRESENT)) {
        pt = (uint64_t *)pmm_alloc_page();
        if (!pt) return;
        for (int i = 0; i < 512; i++) pt[i] = 0;
        pd[pd_index] = ((uint64_t)pt) | PRESENT | WRITE;
    } else {
        pt = (uint64_t *)(pd[pd_index] & ~0xFFFULL);
    }

    pt[pt_index] = phys | flags;
}