#ifndef PMM_H
#define PMM_H

#include "kernel.h"

void pmm_init(EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN MapSize, UINTN DescriptorSize);
uint64_t pmm_alloc_page();
void pmm_free_page(uint64_t addr);
uint64_t pmm_get_free_pages();

#endif